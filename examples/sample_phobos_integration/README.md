This sample involves running Hestia with a [Phobos](https://github.com/cea-hpc/phobos) object store backend.

# Getting the container image

## From Dockerhub

You can get the image on Dockerhub:

```

```

## Building it yourself

To build the image you will need a Phobos rpm for your architecture (aarch64, x86_64 etc) in this directory. You will also need a Hestia binary rpm and Hestia-phobos plugin rpm. To create the latter you can download Hestia source rpm from the releases page: https://git.ichec.ie/io-sea-internal/hestia/-/releases/v1.3.0/downloads/binaries/hestia-1.3.0-1.src.rpm

To build the Hestia binary rpms with the phobos plugin you can do:

```sh
rpmbuild --rebuild -with phobos hestia-1.3.0-1.src.rpm
```

Then you can do:

```sh
podman build -t hestia_phobos .
```

Switching in `docker` is also supported.

# Launching the container

## With podman

To launch a container you can do:

```sh
podman run -v `pwd`:/host -v $HESTIA_CACHE_DIR:/cache -v $PHOBOS_CACHE_DIR:/phobos_mount -p 8080:8080 -p 8090:8090 hestia_phobos
```

where `$HESTIA_CACHE_DIR` is somewhere on the host that you would like the Hestia cache (logs, event feed, object store) to go and similar for the phobos cache and `$PHOBOS_CACHE_DIR`. Port `8080` is the Hestia http interface and `8090` is the s3 interface.

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






