This example demonstrates IO-SEA copytool functionality by moving content to/from a file based store and `Mock` phobos.

# Copytool

The Copytool’s role is to copy objects between different object store backends. 

The copytool implementation is supported by two abstractions: `HSM Object Store Client` interface, an object store supporting some notion of HSM, such as Motr and `Object Store Client` interface representing a standard single tier object store, such as Phobos.
The Copytool uses a HSM Object Store Client interface, with its implementation as a
Hybrid HSM Object Store Client as shown in the figure below, managing other Object Store Clients (e.g. Phobos and Motr) to copy data between them.

![image](CopytoolDesign.png){ width=50% }

# Start a server

Add /path/to/hestia/executable in your PATH. 

```sh
hestia server --config hestia.yaml
```
Here we have configured two backends, the first is simple filesystem-based object store with two tiers and the second one is mock version of phobos. 

# Run the script

Default location of the local cache is in $HOME/.cache/hestia. You may want to clean the previous logs and object and key-value store caches.
You can run the script with:

```sh
sh copytool_sample.sh
```
which will create an object, upload a temporary file to it, add file to Storage Tier tier 0, copy file from tier 0 to tier 1, move from tier 1 to tier 2, copy back from tier 2 to tier 1 and compare the contents of the files on tier 0 and tier 1. 
