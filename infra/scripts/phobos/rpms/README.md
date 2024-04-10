This directory has scripts for building Phobos rpms - this may be useful for Mac 
where it is faster to run VMs and containers on the ARM architecture.

First build the container image:

```sh
podman build -t phobos_build_rpms .
```

Launch the image:

```sh
podman run -it -v `pwd`:/host phobos_build_rpms
```

Build the rpms in the container and copy them to the mounted volume from the host:

```sh
./build_rpms.sh
cp /root/rpmbuild/RPMS/$(arch)/*.rpm /host
```





