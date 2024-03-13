This example demonstrates IO-SEA copytool functionality by moving content to/from a file based store and mock phobos.

# Copytool

The Copytool’s role is to copy objects between Object Store backends. 

The implementation is supported two abstractions: `HSM Object Store Client` interface, an object store supporting some notion of HSM, such as Motr and `Object Store Client` representing a standard single tier object store, such as Phobos.
The Copytool uses a HSM Object Store Client interface, with its implementation as a
Hybrid HSM Object Store Client managing other Object Store Clients.

# Start a server

```sh
cp $HESTIA_SRC_DIR/test/data/configs/s3/s3_server.yaml hestia.yaml
hestia server --config hestia.yaml
```
Here we have configured two backends, the first is simple filesystem-based object store and the second one is mock version of phobos, each with two tiers. 

# Run the script

You can run the script with:

```sh
./hestia_sample.sh
```
which will create an object, upload a temporary file to it, copy file from Storage Tier tier 0 to tier 1 and tier 2, move from tier 1 to tier 3 and  copy back from tier 3 to tier 1.
