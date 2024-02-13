# IO-SEA Quick Start

Thist is a Hestia Quick Start guide with a focus on the needs of the IO-SEA project.

Install the Hestia RPM on a RHEL 8 compatible system:

```bash
sudo dnf install hestia-$VERSION.rpm
```

## Python API

Add the Python package to your `PYTHONPATH`:

```bash
export PYTHONPATH=/usr/lib/hestia/python/hestia:$PYTHONPATH
```

Run the following example script:

```python
import hestia

# Setup some data
my_object_id = "1234"
my_attributes = {
                "key0" : "val0", 
                "key1" : "val1"
                }
my_content = b"content for storage"

# Create the client - this will initialize it too
client = hestia.HestiaClient()

# Create an object with the requested id
client.object_create(my_object_id)

# Add USER attributes to the object
client.object_attrs_put(my_object_id, my_attributes)

# Add data to the object
# Note: there are alternate methods with file paths and file descriptors also
client.object_data_put(my_object_id, my_content)

# Get the USER attributes back
attributes_returned = client.object_attrs_get(my_object_id)
print(f"USER Attributes for object '{my_object_id}':\n{attributes_returned}")

# Get the data back
data_returned = client.object_data_get(my_object_id, len(my_content))
print(f"Data for object '{my_object_id}':\n{data_returned}")
```

as:

```bash
python3 my_script.py
```

The object store, application logs and event feed will be in `$HOME/.cache/hestia`. You can open `$HOME/.cache/hestia/event_feed.yaml` to take a look at the event feed, described more [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/doc/internals/EventFeed.md).

The Python API and more general usage are described in more detail [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/doc/UserGuide.md).

## C API

The IO-SEA C-API is in the file `hestia_iosea.h`. This exclusively uses the lower-level C-API in `hestia.h`, but more closely follows IO-SEA semantics and is easier to use.

There is an example of using it [here](/examples/sample_cmake_app/main_iosea.c).

The [integrations](/src/integrations/README.md) directory has several tools relevant to IO-SEA built on the high-level C-API, they are a good starting point for how to use it in real cases.

The Event Feed is another element of the IO-SEA API - it is documented [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/doc/internals/EventFeed.md).
