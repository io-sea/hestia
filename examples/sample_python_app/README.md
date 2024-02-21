This is a sample Python app for Hestia.

To run it make sure the `hestia` package is your PYTHONPATH. If you built Hestia from source you can add it with:

```bash
export PYTHONPATH=$PYTHONPATH:$HESTIA_BUILD_DIR/$LIB/python/hestia
```

where $LIB is lib64 on Red Hat and lib otherwise. Or if you installed a binary package it will look something like:

```bash
export PYTHONPATH=$PYTHONPATH:/usr/$LIB/hestia/python/hestia
```

The `libhestia` shared library should be found automatically, but if there are issues finding it you can manually add its location to your `LD_LIBRARY_PATH`.

To run the sample do:

```bash
python hestia_sample.py
```
