# Hestia

Hestia (Hierarchical Storage Tiers Interface for Applications) is a [Hierarchical Storage Management](https://en.wikipedia.org/wiki/Hierarchical_storage_management) (HSM) application for Object Stores.

It is being developed in the context of the [IO-SEA](https://iosea-project.eu) project for Exascale Storage I/O and Data Management.

It allows different Object Store backends to be interfaced through a single HSM-enabled API.

Typical use-cases are:

* HSM system implementation
* HSM system benchmarking, anaylsis and design
* Data migration between storage systems

# Core Concepts

The Core Concepts in Hestia are derived from the [IO-SEA](https://iosea-project.eu) project.

In a large-scale storage system it is useful to use the distinct concepts of an `Object Store` and a `Key-Value Store` to implement a File System, rather than a typical monolithic POSIX-implied approach.

User data is kept in an Object Store, with blobs of data labelled with a `unique identifier`. Metadata for describing the File System itself is maintained in a Key-Value store, with just the unique identifiers of the data-blobs in the Object Store referenced.

To support user access and manipulation of large amounts of data it is useful to be able to group objects, in a `Dataset`. It is also necessary to be able to address the data. This is supported via a `Namespace`. In `IO-SEA` and `hestia` a `Namespace` is accociated with a `Dataset`, assigning File-System metadata (such as a path) to Object unique-identifiers. When a consumer mounts a `Namespace` they will load the associated `Dataset` and can address objects as they would in a typical File-System.

`Hestia` is a middleware in the `IO-SEA` software stack, facilititating distributed communication between several other applications. As such it needs to have support for:

* `Key-Value Store` integration
* `Object Store` integration
* `HSM` concepts, such as `Storage Tier`s
* `Dataset` and `Namespace` IO-SEA concepts
* Mappings from standard addressing schemes (e.g. S3, REST) to IO-SEA concepts
* Distributed Communication and Syncronization over a network

Currently the software is an `alpha` state - interfaces are being designed for the above requirements and basic implementations added.

# Runtime Modes

The `hestia` application has four primary runtime modes:

* `Client`: Used to operate directly with Object Stores at a low-level, or with Datasets and Namespaces at a higher level.
* `Client - Standalone`: A single-node version of Hestia, useful for testing.
* `Server - Controller`: Used as an entry-point and co-ordiantor for the Hestia Distributed System. Has direct access to the Key-Value Store interface.
* `Server - Worker`: Accepts commands from the Controller and provides an access-point for large user data access requests. 

In all modes the application can read from a user provided `config` file on the system on launch and maintain a small local `cache` for runtime settings.

# Sample Client Applications

## Low-level CLI

Request an identifier for a new object

```bash
hestia object create
```

The identifier will be returned in the console in [UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier) format :

```bash
>>> 550e8400-e29b-41d4-a716-446655440000
```

Add data from `my_file_in.dat` to this object. By default it will go to the 'fastest' Storage Tier, tier `0`:

```bash
hestia object put 550e8400-e29b-41d4-a716-446655440000 my_file_in.dat
```

Copy the contents of the object to another Storage Tier, tier `1`:

```bash
hestia object copy 550e8400-e29b-41d4-a716-446655440000 0 1
```

Retrieve the content of the object on tier `1` and write it to `my_file_out.dat`:

```bash
hestia object get 550e8400-e29b-41d4-a716-446655440000 my_file_out.dat 1
```

## Low-level REST API

Start the Hestia service

```bash
export HESTIA_ENDPOINT=127.0.0.1:8080
hestia start
```

Create a new object

```bash
curl -X PUT $HESTIA_ENDPOINT/api/v1/hsm/objects
```

```json
{"acl":{"group":[],"user":[]},"created_by":{"id":"00000000-0000-0000-0000-000000000000"},"creation_time":"1687957204420770","dataset_id":"00000000-0000-0000-0000-000000000000","id":"ffffffff-ffff-f719-ffff-fffffffff95a","last_modified_time":"1687957204420770","name":"","read_lock":{"active":"false","locked_at":"0","max_lock_time":"5000"},"size":"0","tiers":[],"type":"hsm_object","user_metadata":{},"write_lock":{"active":"false","locked_at":"0","max_lock_time":"5000"}}
```

The object id will be returned in the `id` field in the response `json`. To add data to the object with id `550e8400-e29b-41d4-a716-446655440000` on tier `0` do:

```bash
curl -X PUT --upload-file "my_file.dat" $HESTIA_ENDPOINT/api/v1/hsm/objects/550e8400-e29b-41d4-a716-446655440000/tiers/0
```

```bash
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100   249    0     0  100   249      0    241  0:00:01  0:00:01 --:--:--   242
```

Copy it to another tier, tier `1`:

```bash
curl -X PUT -H "hestia-hsm-method: copy" \ 
            -H "hestia-hsm-source-tier: 0" \
            $HESTIA_ENDPOINT/api/v1/hsm/objects/550e8400-e29b-41d4-a716-446655440000/tiers/1
```

Retrieve the data from the version on tier `1`:

```bash
curl $HESTIA_ENDPOINT/api/v1/hsm/objects/550e8400-e29b-41d4-a716-446655440000/tiers/1/data
```

Stop the Hestia service

```bash
hestia stop
```

For more details on `Server` setup and other client interfaces like `S3` see the [Hestia User Guide](./doc/UserGuide.md).

# Building from Source

Hestia is supported on Linux (`RHEL 8` is the primary development platform) and Mac. 

## Dependencies

### Required

* A `c++17` compatible compiler
* `cmake >= 3.24` ([script provided](infra/scripts/bootstrap_cmake.sh) to fetch if system version too old)

The following (including development headers):
* `curl`
* `libxml2`
* `OpenSSL` 

Several libraries are also automatically fetched if not found, using CMake `FetchContent`. See the [Hestia Developer Guide](./doc/DeveloperGuide.md) for details.

## Doing the build

It is a 'standard' CMake build, do:

```bash
mkdir $BUILD_DIR; cd $BUILD_DIR
cmake $PATH_TO_SOURCE
make
```

For more build details see the [Hestia Developer Guide](./doc/DeveloperGuide.md).

# License

This code is primarily licensed under an MIT license - see the included LICENSE file for details. Third-party code licenses are included in [external/licenses](external/licenses/).

Some project plugins (lazy-loaded shared libraries) are licensed under the LGPL as required by their specific dependencies - this is determined by their dependecies as listed in their respective CMakeLists file. You can cross check the CMake target for the dependency with the content of [external/licenses](external/licenses/), the directory names map to the CMake target names.