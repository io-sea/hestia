This sample involves running Hestia with a [Phobos](https://github.com/cea-hpc/phobos) object store backend.

# Getting the container image

## From Dockerhub

You can get the image on Dockerhub:

### x86_64 Platforms

```sh
podman pull docker://docker.io/jagrogan/hestia:latest
```

### ARM Platforms

```sh
podman pull docker://docker.io/jagrogan/hestia_arm:latest
```

## Building it yourself

To build the image you will need a Phobos rpm for your architecture (aarch64, x86_64 etc) in this directory. You will also need a Hestia binary rpm and Hestia-phobos plugin rpm. To create the latter on a RHEL 8 x86_64 system you can do:

```sh
./build_hestia_rpms.sh
```

Then you can do:

```sh
podman build -t hestia .
```

To build the image. Switching in `docker` is also supported. Swap `hestia` with `hestia_arm` on ARM systems.

# Launching the container

## With podman

To launch a container you can do:

```sh
podman run -v $HESTIA_CACHE_DIR:/cache -v $PHOBOS_CACHE_DIR:/phobos_mount -p 8080:8080 -p 8090:8090 hestia
```

where `$HESTIA_CACHE_DIR` is somewhere on the host that you would like the Hestia cache (logs, event feed, object store) to go and similar for the phobos cache and `$PHOBOS_CACHE_DIR`. Port `8080` is the Hestia http interface and `8090` is the s3 interface. Make sure the cache directories are empty on each launch.

Swap `hestia` with `hestia_arm` on ARM systems.

## With compose

You can also use `podman-compose` which will take care of the mounting and ports for you. First make directories for the caches and then launch with compose:

```sh
mkdir hestia_cache
mkdir phobos_cache
podman-compose  -f compose.yaml up -d
```

and to shut down:

```sh
podman-compose -f compose.yaml down -v
```

# Manipulating objects

With the server running in the container you can use the Hestia client (or curl as per examples in the `sample_shell` directory) in the host to manipulate objects. For example

```sh
hestia object create 1234 --host 127.0.0.1
echo "test data" > test_file.dat
hestia object put_data --file test_file.dat 1234 --host 127.0.0.1
```

Notice that logs, event feed and object store have all now been updated in `$HESTIA_CACHE_DIR`.

We will now use the `copytool` to move objects to the Phobos object store (tier). 

```sh
hestia object move_data --source 0 --target 4 1234 --host 127.0.0.1
```

Note that a new directory will have been created in `$PHOBOS_CACHE_DIR`, holding the object data.

Note: the `copy` operation currently doesn't work between container and host mounts due to an issue with a system library.

We can now read the object content back from the phobos tier to the host:

```sh
hestia object get_data --tier 4 --file test_file_ret.dat 1234 --host 127.0.0.1
```

You can compare the two files to check that they are the same:

```sh
diff test_file.dat test_file_ret.dat
```





