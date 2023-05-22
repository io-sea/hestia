# Hestia

Hestia (Hierarchical Storage Tiers Interface for Applications) is a [Hierarchical Storage Management](https://en.wikipedia.org/wiki/Hierarchical_storage_management) (HSM) interface for Object Stores.

It allows different Object Store backends to be accessed through a single HSM API - via command line interface or network requests.

Typical applications are:

* Data migration between storage systems
* HSM system implementation
* HSM system benchmarking, anaylsis and design

# Sample Applications

## Config 

Specify which HSM 'Tiers' are handled by which Object Stores via a `yaml` config:

```yaml
tier_registry:
  - identifier: 0
    client_identifier: hestia::MotrClient
  - identifier: 1
    client_identifier: hestia::MotrClient
  - identifier: 2
    client_identifier: hestia::S3Client
object_store_clients:
  - identifier: hestia::S3Client
    source: plugin
    type: basic
    plugin_path: libhestia_s3_plugin
    host: 127.0.0.1:8000
  - identifier: hestia::MotrClient
    source: plugin
    type: hsm
    plugin_path: libhestia_motr_plugin
```

## CLI

Add an object `00001234` to tier `0`:

```bash
hestia put 00001234 my_file_in.dat 0 --config=hestia.yaml
```

Copy it to another tier, tier `1`:

```bash
hestia copy 00001234 0 1 --config=hestia.yaml
```

Retrieve the version on tier `1`:

```bash
hestia get 00001234 my_file_out.dat --config=hestia.yaml
```

## Http

```yaml
server:
  host_address: 127.0.0.1
  host_port: 8080
  web_app: hestia::HsmService
  backend: hestia::Basic
  ...
```

Start the Hestia service

```bash
export HESTIA_ENDPOINT=127.0.0.1:8080
hestia start --config=hestia.yaml
```

Add an object `00001234` to tier `0`:

```bash
curl -X PUT --upload-file "my_file.dat" \ $HESTIA_ENDPOINT/api/v1/hsm/tiers/0/objects/000012345/data
```

Copy it to another tier, tier `1`:

```bash
curl -X PUT -H "hestia-hsm-method: copy" \ 
            -H "hestia-hsm-source-tier: 0" \
            $HESTIA_ENDPOINT/api/v1/hsm/tiers/1/objects/000012345
```

Retrieve the version on tier `1`:

```bash
curl $HESTIA_ENDPOINT/api/v1/hsm/tiers/1/objects/000012345/data
```

List the supported object store backends on the system - the node supporting the backend may be elsewhere on
the network.

```bash
curl $HESTIA_ENDPOINT/api/v1/object_store_backends/
```

List the hestia nodes in the system:

```bash
curl $HESTIA_ENDPOINT/api/v1/nodes/
```

Stop the Hestia service

```bash
hestia stop
```

## S3

This example uses Amazon's `boto` Python S3 client library: `pip install boto3`

```yaml
server:
  host_address: 127.0.0.1
  host_port: 8080
  web_app: hestia::HsmS3Service
  backend: hestia::Basic
  ...
```

Start the Hestia service

```bash
hestia start --config=hestia.yaml
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

For more details see the [Hestia User Guide](./doc/UserGuide.md).

# Building from Source

Hestia is supported on Linux (`Rocky 8` is the primary platform) and Mac. 

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