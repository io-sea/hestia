# Guide to the building and consumption of Motr in Hestia

[Cortx-Motr](https://github.com/Seagate/cortx-motr) is a key dependency of Hestia,
managing the HSM of objects from Archival HDD tiers to NVME SSD. There are no
official Motr RPM builds, so we package our own for use in this project.

RPMs are built on Rocky 8, in a Docker container (the CI). Aarch64(Arm64) builds
are also run for use in development on Mac silicon (within a rocky8 container).

## Building Motr

See the documentation on the branch [motr-rpm-builds](https://git.ichec.ie/io-sea-internal/hestia/-/blob/motr-rpm-builds/README.md).
In summary, to get a new motr build, edit the version number in 
`infra/scripts/build_motr.sh`, and push the commit. Then run the job `motr_rpm_build`
in the CI to publish the new build to the [package repository](https://git.ichec.ie/io-sea-internal/hestia/-/packages/33). 

## Consuming Motr

The `motr-rpm-builds` branch publishes RPMs as `tar.gz` files to the `motr-rpms` 
package in the project package repository. It does this using the Gitlab API, which 
we also use to download these files for consumption. 

This feature is demonstrated in the `use-motr-ci` branch, in the job 
[motr install test](https://git.ichec.ie/io-sea-internal/hestia/-/packages/33).
This job uses the following snippet to install motr from the package repo. Note, if 
you are running this locally you will need to use your own auth tokens. 

``` yaml
 - &install_motr
    curl ${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/generic/motr-rpms/1.0.0/motr-rpms-$(arch).tar.gz | tar xz;
    yum install -qy ${CI_PROJECT_DIR}/motr-rpms/isa-l-2.30.0*.$(arch).rpm;
    yum install -qy ${CI_PROJECT_DIR}/motr-rpms/cortx-motr-2.0.0*.$(arch).rpm;
    yum install -qy ${CI_PROJECT_DIR}/motr-rpms/cortx-motr-devel-2.0.0*.$(arch).rpm;

```

Note, for the packages `cortx-utils` and `cortx-hare` (only available on x86_64), you 
will need to manually install additional dependencies before installing the RPMs. 
Again, these instructions are for Rocky8.

### Cortx-Utils

```bash
export HOST_ARCH=$(arch)
export CORTX_UTILS_SRC="CORTX-UTILS SOURCE DIR"

python3 -m pip install --upgrade pip
python3 -m pip install --no-cache-dir asciidoc ply cffi==1.14.5 numpy==1.19.5

if [ $HOST_ARCH == "aarch64" ];then
    # On aarch64 nothing gives kafka python wheel and repo lib is too old, so need to build.
    cd /;curl -Ls https://github.com/confluentinc/librdkafka/archive/refs/tags/v1.5.0.tar.gz | tar xz;cd librdkafka-1.5.0;
    ./configure --install-deps;make -j$(nproc);make install
    python3 -m pip install confluent-kafka==1.5.0
fi 

pip3 install -r $CORTX_UTILS_SRC/py-utils/python_requirements.txt

```

### Cortx-Hare

```bash
    yum-config-manager --add-repo https://rpm.releases.hashicorp.com/RHEL/hashicorp.repo # Provides consul dependency
```
