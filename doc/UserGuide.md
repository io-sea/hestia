# Hestia User Guide

`Hestia` (Hierarchical Storage Tiers Interface for Applications) is a [Hierarchical Storage Management](https://en.wikipedia.org/wiki/Hierarchical_storage_management) (HSM) interface for Object Stores.

It can be used to interact with different Object Stores with the same API, including standard CRUD operations for objects and data migration between Stores or Tiers in HSM systems.

This is the `Hestia` user guide, it covers:

* The `Hestia` Config file format
* Command Line Interface
* C++ Interface
* Http Interface
* S3 Interface

# Config File Format

## Object Store Backend Settings

Hestia uses a `yaml` format for runtime configuration. The most important section is the `tier_registry`, for example:

```yaml
tier_registry:
  - identifier: 0
    client_identifier: hestia::FileHsmObjectStoreClient
  - identifier: 1
    client_identifier: hestia::FileHsmObjectStoreClient
  - identifier: 2
    client_identifier: hestia::S3Client
```

This is a sequence of maps from a HSM tier `identifier` (0-255) to a string `client_identifier`. The `client_identifier` in turn maps into a sequence in the `object_store_clients` section. Each `object_store_clients` entry gives setup information needed for a particular Object Store backend.

```yaml
object_store_clients:
  - identifier: hestia::FileHsmObjectStoreClient
    source: built_in
    type: hsm
    root: hsm_object_store
  - identifier: hestia::S3Client
    source: plugin
    type: basic
    plugin_path: libhestia_s3_plugin
    host: 127.0.0.1:8000
```

The following options are common to all backends:

* `source` : `built_in`, `plugin` - whether the backend comes 'built in' to hestia or is loaded as a plugin (shared library)
* `type` : `basic`, `hsm` - whether the backend supports HSM operations (copy or move between storage tiers)

The following backends are currently supported, prefixed with `hestia::` (the [Developer Guide](DeveloperGuide.md) gives details on adding custom backends with the `plugin` system):

* `FileObjectStoreClient` : A client with object stored in the local filesystem - this is mostly intended for testing/simulation
* `S3Client` : A client for S3 endpoints - implemented using [libs3](https://github.com/bji/libs3)
* `PhobosClient`: A client for the [Phobos](https://github.com/cea-hpc/phobos) library for tape-backed Object Stores
* `FileHsmObjectStoreClient` : A HSM enabled version of the `FileObjectStoreClient`
* `MotrClient` : A client for the HSM enabled [Motr](https://github.com/Seagate/cortx-motr) Object Store

In addition, a number of `Mock` versions of the above clients are available for testing and simulation.

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

# Command Line Interface

The command line interface (CLI) provides a HSM API for the Object Store backends specified in the `yaml` config file. It can be used as follows:

```bash
hestia put 00001234 my_file_in.dat 0 --config=hestia.yaml
```

Here:
* `put` is the HSM method
* `00001234` is the object id - it can be any 'key', it is up to the user to ensure uniqueness, such as via UUID.
* `my_file_in.dat` is the path to the file with object data on the local system
* `0` is the HSM Tier to place the object on (0-255). Defaults to 0.
* `--config` is an optional (but likely required) path to the Hestia config file.

The following HSM methods are currently supported (* is optional):

* `put <object_id> <file_path> <target_tier_id*>`
* `get <object_id> <file_path> <source_tier_id*>`
* `copy <object_id> <source_tier_id> <target_tier_id>`
* `move <object_id> <source_tier_id> <target_tier_id>`

# C++ Interface

The C++ interface can be consumed via the `hestia.h` header and linking to the project binaries. Linking is easiest via CMake - the `find_project(hestia)` API is supported - providing the `hestia::hestia` CMake target.

This API provides similar functionality to the CLI.

# Web Interfaces

To start a Hestia Web Service you can do:

```bash
hestia server --config=hestia.yaml
```

this will block in the terminal while the service is running.