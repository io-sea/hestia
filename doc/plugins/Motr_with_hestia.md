Download motr rpms from hestia repo [Package Registry](https://git.ichec.ie/io-sea-internal/hestia/-/packages), under Deploy > Package registry  

To install motr from the rpms provided in the hestia gitlab package registry, download the tarball from the repository and expand it, then run the following script (found in infra/scripts), ensuring that the argument passed in is the path to the top level 'builds' folder

```bash 
    sudo sh install_motr_rpms.sh path/to/builds/folder    
```

You can verify Your network protocol and interface were set correctly using:  

```bash 
cat /etc/libfab.conf
```
and see that the output is similar to

```bash
networks=tcp(enp0s3)
```

If this has not been set, use the `ifconfig` or `ip` commands to find your network interface and protocol and populate the config file. Use this to edit the file `external/motr/singlenode-multipools.yaml` [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/external/motr/singlenode-multipools.yaml?ref_type=heads) to show the correct network protocol on line 7: 

```bash
data_iface: enp0s3  #or eth0 etc
```

Next run: 

```bash 
/path/to/hestia/infra/scripts/motr_setup.sh
```

This will create the folder var/motr in your home directory, start the cluster and output the config section to be added to the hestia config file. 

Build hestia against motr, using the script in the same directory: 

```bash
/path/to/hestia/infra/scripts/build_hestia_with_motr.sh
```

This will create the build directory under hestia, and pull the motr source code in there. If you are not using rpms of the latest version, edit the SHA online 14

Set the library path to pick up the hestia libraries

```bash
export LD_LIBRARY_PATH=/path/to/build/lib
```

Add the output from hctl status to the hestia config file, under the header `object store clients`. An example can be found in `test/data/configs/motr/hestia_motr_tests.yaml` [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/test/data/configs/motr/hestia_motr_tests.yaml?ref_type=heads).

Start the hestia server with this config file. To add the 

```bash 
path/to/hestia/build/bin/hestia server --config=path/to/motr/config.yaml
```

