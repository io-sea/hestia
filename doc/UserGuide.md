# Hestia User Guide

`Hestia` (Hierarchical Storage Tiers Interface for Applications) is a [Hierarchical Storage Management](https://en.wikipedia.org/wiki/Hierarchical_storage_management) (HSM) interface for Object Stores.

It can be used to add HSM functionality to standard Object Stores, as well as grouped operations on collections of Objects (`Dataset` feature) and general addressing (S3, POSIX etc) via a `Namespace`s feature.

This is the `Hestia` user guide, it covers:

- [Hestia User Guide](#hestia-user-guide)
- [Core Concepts](#core-concepts)
  - [Data Model](#data-model)
- [Runtime Modes](#runtime-modes)
- [Configuration](#configuration)
  - [Object Store Backend Settings](#object-store-backend-settings)
  - [HSM Settings](#hsm-settings)
  - [Server Settings](#server-settings)
  - [Event Feed](#event-feed)
- [APIs](#apis)
  - [Command Line Interface](#command-line-interface)
  - [C and Python Interfaces](#c-and-python-interfaces)
  - [Web Interfaces](#web-interfaces)
    - [REST API](#rest-api)
- [S3 API](#s3-api)
- [Ansible Deployment](#ansible-deployment)
  - [Usage](#usage)
    - [Hestia Roles in your Playbook](#hestia-roles-in-your-playbook)
  - [Configuration](#configuration-1)
    - [Backend Types/Configurations](#backend-typesconfigurations)

# Core Concepts

The Core Concepts in Hestia are derived from the [IO-SEA](https://iosea-project.eu) project.

Hestia provides `Object Store` and a `Key-Value Store` abstractions. Data blobs are kept in an Object Store, addressed with a `unique identifier`. `Metadata` is maintained in a Key-Value store, referencing the identifiers of the data-blobs in the Object Store.

Objects can be grouped in a `Dataset`. Object addressing in a Dataset is through `User` scoped keys or paths, which are defined via a `Namespace`. This concept has parallels with `S3` buckets and objects, but allows for more general addressing such as with POSIX filesystem semantics.

`Hestia` provides these abstractions via a middleware functionality in a distributed storage setting, supporting:

* `Key-Value Store` integration
* `Object Store` integration
* `HSM` concepts, such as `Storage Tier`s
* `Dataset` and `Namespace` IO-SEA concepts
* Mappings from standard addressing schemes (e.g. S3, REST) to IO-SEA schemes
* Distributed Communication and Syncronization over a network

## Data Model
The figure below shows the abstractions used in Hestia and their relationships. 

![image](images/HestiaDataModel.png)

* `Node`: This is a Hestia server instance - it holds server address details and configuration of the application running on the server
* `Tier` : This is a Storage Tier in a HSM system - it represents a physical storage resource, with descriptive properties such as bandwitdh, capacity, storage media etc.
* `User` : Several Hestia resources are scoped to a user - they can be registered in Hestia with a unique username and authenticate with a system provided token.
* `Event` : Any events (such as resource creation, update, destruction etc) can be output to a feed for system analytics or Data Warehousing applications.
* `Dataset` : A Dataset is a group of Objects owned by a User, allowing for Object operation batching and hints for automated HSM movements.
* `Action` : A HSM action (data put, get, copy, move etc) requested by a User, it provides tracking and progress updates for long running operations.
* `Object` : A Storage object - it can point to a single object with data split over several storage tiers and also hold object metadata.
* `Namespace` : An addressing scheme (e.g. POSIX path) and permissions for Objects in a Dataset.
* `Extent` : An Object's data on a particular Storage Tier - it is a collection of block offsets and lengths, allowing random access for data put and get operations where supported by the third-party object store backend.
* `Metadata` : A collection of key-value pairs representing metadata for Object, Datasets etc - it is split from the parent model to allow for quicker access or indexing.

In the figure solid lines represent parent-child relationships, which are one-to-many unless explicitly specified one-to-one (x1). 

The Hestia API is primarily [CRUD](https://en.wikipedia.org/wiki/Create,_read,_update_and_delete)-like. Resources are most efficiently addressed via their unique identifier (or primary key) - but for convenience addressing via a human-readable `name` and reference to a `parent` is also supported. In all Hestia APIs the abstractions above are referenced in lower case (e.g. `node`, `tier`, `user`).

A typical CRUD operation is structured as `subject method id`, for example to create an object with a requested unique identifier would be:

```
object create 550e8400-e29b-41d4-a716-446655440000
```

Via `parent` addressing this would look like:

```
object create name=my_object_name parent=my_dataset_name
```

where the `User` parent of the referenced `Dataset` is inferred from a provided authentication token.


# Runtime Modes

The figure below shows an example application of Hestia in the IO-SEA project. In this scenario Hestia runs in several modes, including acting as a `Client` when putting and getting data to-and-from remote Long-Term Storage and as a `Server` when coordinating data streaming to remote workers interfacing third-party object stores on their nodes.

![image](images/IOSEA_Arch.png)

Overall, the `hestia` application has four runtime modes:

* `Client`: Used to operate directly with Object Stores at a low-level, or with Datasets and Namespaces at a higher level.
* `Client - Standalone`: A single-node version of Hestia, useful for testing.
* `Server - Controller`: Used as an entry-point and co-ordiantor for Hestia in a distributed system. Has direct access to the Key-Value Store interface.
* `Server - Worker`: Accepts commands from the Controller and provides an access-point for data streaming. 

In all modes the application can read from a user provided `config` file on the system on launch and maintain a light local `cache`.

# Configuration

Internally Hestia uses a custom serialization format which is a subset of JSON to convert between C++ objects and strings. As a result, many Hestia resources can be configured in a fine-grained way by serializing user-provided configs in YAML or JSON format.

The most direct way of providing a configuration is via a `yaml` file - which can be read from disk on launch. When launched as a systemd service, Hestia will use the config at `/etc/hestia/hestiad.yaml`, which provides some helpful defaults. 

Since the configuration schema is derived directly from the C++ code, the source code itself is the primary source of documentation for this yaml file, however some core settings are covered here as examples.

The internal Hestia architecture shown below may be a useful resource when searching the project for configuration options:

![image](images/HestiaArch.png)

## Object Store Backend Settings

Hestia can interface with several Object Store backends at the same time, thus their configuration is provided as a Sequence of `object_store_client` types:

```yaml
object_store_clients:
  - identifier: hestia::FileObjectStoreClient
    backend_type: file
    config:
      root: object_store
  - identifier: hestia::S3Client
    backend_type: s3
    plugin_path: libhestia_s3_plugin
    config:
      host: 127.0.0.1:8000
```

Here we have configured two clients, one to a simple built-in Object Store which uses the local file-system and the other to a remote store with an S3 interface. We provide an arbitary `identifier` to address the clients at runtime, a `backend_type` to give a hint on the nature of the client, and a client-specific `config`. For clients provided as a plugin (shared library) we can provide an optional `plugin_path` search hint.

The following backends are currently supported:

* `FileObjectStoreClient` : A client with object stored in the local filesystem - this is mostly intended for testing/simulation
* `S3Client` : A client for S3 endpoints - implemented using [libs3](https://github.com/bji/libs3)
* `PhobosClient`: A client for the [Phobos](https://github.com/cea-hpc/phobos) library for tape-backed Object Stores
* `FileHsmObjectStoreClient` : A HSM enabled version of the `FileObjectStoreClient`
* `MotrClient` : A client for the HSM enabled [Motr](https://github.com/Seagate/cortx-motr) Object Store

In addition, a number of `Mock` versions of the above clients are available for testing and simulation.

## HSM Settings

When working with HSM systems we want to match a `Storage Tier` with an Object Store backend that than handle data operations for this tier, we can create this relationship in the config as follows:

```yaml
tiers:
  - identifier: 0
    client_identifier: hestia::FileHsmObjectStoreClient
  - identifier: 1
    client_identifier: hestia::FileHsmObjectStoreClient
  - identifier: 2
    client_identifier: hestia::S3Client
```

This maps a `Storage Tier` with a system `identifier` (0-255) to a `client_identifier` specified when defining the `object_store_clients` entries. Note that a single HSM enabled object store client can be mapped to from multiple Storage Tiers.

## Server Settings

Hestia can be used via a `Command Line Interface`, `C++ API` or over a network. The network API is available as `Http` or `S3`. The `yaml` file allows specification of server configuration as follows:

```yaml
server:
  host_address: 127.0.0.1
  host_port: 8080
  web_app: hestia::HsmService
  backend: hestia::Basic
```

The server will attempt to bind at the `host_address` and `host_port` when started. The `web_app` specifies which `Hestia` application to launch on the server. It currently supports:

* `hestia::HsmService` : The `HTTP` interface to the Hestia - this exposes HSM operations as per the CLI (detailed later), but also a co-ordination API for distributed `Hestia` instances.
* `hestia::HsmS3Service` : The `S3` interface to Hestia - this exposes an S3 Object Store interface with optional HSM support via header metadata.

Hestia currently supports two server backends with the `backend` option:

* `basic` : Use a simple (custom written) server implementation - this is very basic - only intended for local development and testing
* `proxygen` : User a server built on the [Proxygen](https://github.com/facebook/proxygen) library

## Event Feed

Hestia will optinally log resource-altering events to a YAML file specified by `event_feed_path` in the general config as shown below. 

```yaml
event_feed:
  event_feed_path: event_feed.yaml
  event_feed_active: y
```

For an annotated example of the output of this event logger please see [here](Markdown/event_feed_format.md).  

# APIs

## Command Line Interface

The `hestia` application provides a command line interface supporting:

* CRUD operations on all Hestia abstractions (models)
* A HSM Action (put, get, copy, move, release data) API for Objects and Datasets
* Server and Deamon control

A simple example is shown in the top-level README in the project.

The CRUD interface is low-level and general to allow easier adaptation as the needs of the parent IO-SEA project change. Since the `hestia` application is launched once per operation it is designed to support batch-inputs.

The main flags are:

* `input_fmt`: Specifies whether json or a combination of unique-identifiers and key-value pairs are expected in the input.
* `output_fmt`: Requests the output to be in json or a combination of unique identifiers and key-value pairs.
* `id_fmt`: Specifies whether input identifiers are in the format of unique-identifiers or human readable `names` and `parent` names (see notes on Addressing in the Core Concepts section above).

Depending on the input form specifiers input will be expected as trailing command-line arguements or via `stdin` on launch. A typical batch run would have a format like:

```bash
hestia object create --input_fmt=key_value --output_fmt=json < requested_object_ids.dat > my_new_objects.json
```

with `requested_object_ids.dat` being a new-line-separated sequence of unique identifiers.

When running from the CLI a local `cache` will be created - with a default location in `$HOME/.cache/hestia`. This will contain application logs and, if configured, object and key-value store caches.

## C and Python Interfaces

The C interface can be consumed via the `hestia.h` header and linking to the project binaries. Linking is easiest via CMake - the `find_project(hestia)` API is supported - providing the `hestia::hestia` CMake target. The header file documentation is the recommended reference for its use.

For similar reasons to the CLI, the interface is designed to be adaptable rather than rigid due to the fluid requirements of the parent project - these is manifested by relying on strings and format specifiers as primary input and output arguments.

The Python interface is a low-level wrapping of the C interface via `ctypes` - located in the `bindings` directory. It requires the `hestia_lib` to be in the system library search paths.

To test your setup you can run:

```bash
cd bindings/python/hestia
export PYTHONPATH=$PYTHONPATH:`pwd`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/$HESTIA_BUILD_DIR/lib
python3 test/test_binding.py
```

(on Mac set the `DYLD_LIBRARY_PATH` instead)

Overall the semantics of the C interface are similar to the CLI - with a `hestia_subject_method` pattern for CRUD operations. 

## Web Interfaces

Hestia provides two primary web-interfaces (S3 and generic REST) through which a number of web-apps (`Worker` and `Controller`-like) can be run. 

The web-apps are run on a Proxygen web-server (with a basic custom-built server provided for testing).

The server can be run blocking or as a daemon depending on the supplied command line flags - with the loaded app controlled by the supplied `yaml`
 config.

### REST API
As an example of a REST API, start the Hestia service

```bash
export HESTIA_ENDPOINT=127.0.0.1:8080
hestia start
```

Create a new object

```bash
curl -X PUT $HESTIA_ENDPOINT/api/v1/objects
```

```json
{"acl":{"group":[],"user":[]},"created_by":{"id":"00000000-0000-0000-0000-000000000000"},"creation_time":"1687957204420770","dataset_id":"00000000-0000-0000-0000-000000000000","id":"550e8400-e29b-41d4-a716-446655440000","last_modified_time":"1687957204420770","name":"","read_lock":{"active":"false","locked_at":"0","max_lock_time":"5000"},"size":"0","tiers":[],"type":"hsm_object","user_metadata":{},"write_lock":{"active":"false","locked_at":"0","max_lock_time":"5000"}}
```

The object id will be returned in the `id` field in the response `json`. To add data to the object with id `550e8400-e29b-41d4-a716-446655440000` we create a HSM action:

```bash
curl -X PUT --upload-file "my_file.dat" \
  -H "hestia.hsm_action.action: put_data" \
  -H "hestia.hsm_action.subject_key: 550e8400-e29b-41d4-a716-446655440000" \
  $HESTIA_ENDPOINT/api/v1/actions/
```

Note the use of the HTTP Header to provide metadata as the body is reserved for object data streaming. The header is a direct serialization of the C++ `Action` instance - the corresponding source code documentation can be used for reference.

```bash
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100   249    0     0  100   249      0    241  0:00:01  0:00:01 --:--:--   242
```

Copy it to another tier, tier `1`:

```bash
curl -X PUT -H "hestia.hsm_action.action: copy_data" \ 
            -H "hestia.hsm_action.source_tier: 0" \
            -H "hestia.hsm_action.target_tier: 1" \
            -H "hestia.hsm_action.subject_type: object" \
            -H "hestia.hsm_action.subject_key: 550e8400-e29b-41d4-a716-446655440000" \
            $HESTIA_ENDPOINT/api/v1/actions
```

Retrieve the data from the version on tier `1`:

```bash
curl -H "hestia.hsm_action.source_tier: 1" \
     -H "hestia.hsm_action.subject_key: 550e8400-e29b-41d4-a716-446655440000" \
  $HESTIA_ENDPOINT/api/v1/hsm/actions
```

Stop the Hestia service

```bash
hestia stop
```

# S3 API

This example uses Amazon's `boto` Python S3 client library: `pip install boto3`

Start the Hestia service

```bash
hestia start
```

Add an object `00001234` to tier `0`:

```python
import boto
session = boto3.session.Session(aws_access_key_id="OPEN_KEY", aws_secret_access_key="SECRET_KEY")
client = session.client(service_name='s3', endpoint_url="HESTIA_ENDPOINT")

client.create_bucket(Bucket="my_bucket")

client.upload_file(Filename=filename,
  Bucket=bucket_name,
  Key=key,
  ExtraArgs={"Metadata": meta_data})
```

Copy it to another tier, tier `1`:

```python
boto.post()
```

Retrieve the version on tier `1`:

```python
boto.get()
```

Stop the Hestia service

```python
hestia stop
```

# Hestia as a systemd service

When installed as an RPM package, Hestia will install as service unit file for launching the server, and default configuration for this server at `/etc/hestia/hestiad.yaml`.

This service can be controlled as normal using systemd for example:

```bash
systemctl enable hestiad # The hestia server will be started on login
systemctl start hestiad # Start the server (once-off)
systemctl status hestiad # Check the server's status
systemctl restart hestiad # Restart the server (will reload the configuration file)
```

The server can be interfaced with as described in the various APIs above. All logs will be written to the system log, which can be viewed with `journalctl`.  
# Ansible Deployment

Ansible roles featuring scripts and configuration templates for Hestia's deployment on a multi-node cluster are demonstrated [here](/examples/sample_ansible_deploy/playbook.yml).  
## Usage

### Hestia Roles in your Playbook

As specified in [the example playbook](/examples/sample_ansible_deploy/playbook.yml), we can use the roles `hestia_controller_node` and `hestia_worker_node` given to setup a hestia cluster consisting of one or more controller nodes, each of which owns one or more worker nodes. By default, this will use the Redis key-value store on the controller node.

The playbook syntax to use these roles to configure your controller and worker is as follows, assuming you have a group of hosts defined for `hestia_controllers` and `hestia_workers` in your inventory: 

```yaml
---
- hosts: hestia_controllers
  roles:
    - hestia_controller_node

- hosts: hestia_workers
  roles:
    - hestia_worker_node
```

## Configuration

An example inventory for a three node test setup is provided. The following variables are supported when provided in your inventory variables and are propogated to the relevant config files on the nodes. Specifiying different values for different groups is possible as detailed in the ansible documentation and can even be set differently for individual nodes.

| Variable Name | Default Value | Notes |
|---|---|---|
| `iosea_repo_url` | `http://www.somerepo.com/iosea` | The url of the YUM repository including the `hestia` package |
| `hestia_server_port` | `8080` | The port used for all Hestia servers on nodes |
| `hestia_controller_address` | `hestia-controller` | The address of the controller node for the targeted workers |
| `hestia_hsm_tiers` | 5 File HSM backend tiers | List of tier names and their [backend types](#object-store-backend-settings) |
| `hestia_backends` | File HSM default config | A list of configurations sent to all target workers with the listed backend |


### Backend Types/Configurations

Configuration options for all backends are given under the `config` map in the `hestia_backends` option, as in the regular configuratiion files for Hestia. For example: 
```yaml
  hestia_backends:
    - type: "file_hsm"
      config: 
        - hsm_root: "hsm_object_store"
``` 