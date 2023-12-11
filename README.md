# Hestia

Hestia (Hierarchical Storage Tiers Interface for Applications) is a [Hierarchical Storage Management](https://en.wikipedia.org/wiki/Hierarchical_storage_management) (HSM) wrapper for Object Stores.

It is being developed in the context of the [IO-SEA](https://iosea-project.eu) project for Exascale Storage I/O and Data Management.

Currently the software is in a `beta` state - interfaces are being designed for use in the IO-SEA project and basic implementations added.

# Hello World Example

The `--help` flag gives the CLI options:

```bash
hestia --help
```

Create a new object:

```bash
hestia object create
```

A unique identifier for the object will be returned in the console:

```bash
>>> 550e8400-e29b-41d4-a716-446655440000
```

Add data from `my_file_in.dat` to this object. By default it will go to the 'fastest' Storage Tier, tier `0`:

```bash
hestia object put_data 550e8400-e29b-41d4-a716-446655440000 --file my_file_in.dat
>>> 43e006fa-8ca7-d57b-6c5b-491a0881dc9f
```

A unique identifier for the corresponding `HsmAction` will be returned in the shell, it can be queried to get the status and further information on the transfer:

```bash
hestia action read 43e006fa-8ca7-d57b-6c5b-491a0881dc9f
```

We can also query the object to see where the data has been written to:

```bash
hestia object read 550e8400-e29b-41d4-a716-446655440000
```

Next, copy the contents of the object to another Storage Tier, tier `1`:

```bash
hestia object copy_data 550e8400-e29b-41d4-a716-446655440000 --source 0 --target 1
```

Retrieve the content of the object on tier `1` and write it to `my_file_out.dat`:

```bash
hestia object get_data 550e8400-e29b-41d4-a716-446655440000 --tier 1 --file my_file_out.dat 
```

Now, add some user metadata as a key-value pair (`my_key0=my_value0`) to the object. Note the use of the `data.` prefix on the key, this is for denoting user/non-system attributes:

```bash
hestia metadata update --id_fmt=parent_id --input_fmt=key_value 550e8400-e29b-41d4-a716-446655440000 <<<data.my_key0=my_value0
```

Here we are reading the `metadata` type, but referencing the unique identifier of its parent object with `--id_fmt=parent_id`. We use the `heredoc <<<` functionality of the shell for the example, put typically you would pipe in the content of a file.

Finally we can read it back:

```bash
hestia metadata read --id_fmt=parent_id --query_fmt=id --output_fmt=key_value 550e8400-e29b-41d4-a716-446655440000
```

Here we use `--query_fmt=id` to specify that we want to do a search using 'ids' and `--id_fmt=parent_id` to specify that we are searching using the id of the metadata item's parent object. Parent-child relationships for Hestia types are covered in the [Hestia User Guide](./doc/UserGuide.md).

Hestia provides several interfaces with similar functionality to the CLI, including:

* [c and Python APIs](/doc/UserGuide.md#c-interface)
* [HTTP REST API](/doc/UserGuide.md#rest-api)
* [S3 API](/doc/UserGuide.md#s3-api)

For more details and sample applications see the [Hestia User Guide](./doc/UserGuide.md).

# Installation

## Binary Packages

RPM and tarball binary packages for `RHEL 8` are available in the [Hestia repo tagged releases](https://git.ichec.ie/io-sea-internal/hestia/-/releases).

## Building from Source

Linux (`RHEL 8` is the primary development platform) and MacOS are supported.

### Dependencies

#### Required

* A `c++17` compatible compiler
* `cmake >= 3.24`. NOTE: This is currently newer than the system version in many distros. A [script is provided](infra/scripts/bootstrap_cmake.sh) to fetch it if the system version is too old.
* `git`
* `make`
* A web connection - unless further build dependencies are manually installed (see the [Hestia Developer Guide](./doc/DeveloperGuide.md) for details).

### Doing the build

Do:

```bash
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake $PATH_TO_SOURCE
make
```

For more build details see the [Hestia Developer Guide](./doc/DeveloperGuide.md).

# License

This code is primarily licensed under an MIT license - see the included LICENSE file for details. Third-party code licenses are included in [external/licenses](external/licenses/).

Some project plugins (lazy-loaded shared libraries) are licensed under the LGPL as required by their specific dependencies - this can be determined in their respective CMakeLists files. You can cross check the CMake target for the dependency with [external/licenses](external/licenses/), the directory names map to the CMake target names.