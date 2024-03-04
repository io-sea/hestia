Download motr rpms from hestia repo, under Deploy > Package registry. Set an environement variable for their location as 

Note: If you have not yet installd the hestia base dependencies, do so before installing the rpms.

```bash
    cd hestia
    yum install -qy epel-release
    yum config-manager --set-enabled powertools
#    yum install -qy $(cat infra/deps/rocky8/base)
```

To install motr from the rpms provided in the hestia gitlab package registry, download the tarball from the repository and expand it, then run the following script (found in infra/scripts), ensuring that the argument passed in is the path to the top level 'builds' folder

```bash 
    sudo sh install_motr_rpms.sh path/to/builds/folder    
```

Download the iosea motr vm at https://github.com/Seagate/cortx-motr/releases/tag/iosea-vm-v2.1, and use the README.txt file and enclosed scripts to start the motr cluster. Run the libfab-config.sh script and pass your network protocol and interface as arguments. These can be found using the ip command. For example: 


```bash
sudo sh libfab-config.sh tcp enp0s3
```

You can verify this step using: 

```bash 
cat /etc/libfab.conf
```
and see that the output is 

```bash
networks=tcp(enp0s3)
```

Next download the disks.sh script and run: 

```bash 
./disks.sh
```

This will create the folder var/motr in your home directory. 

Download the singlenode.yaml file and replace instances of /home/seagate with the path to your home folder, or wherever your var/motr directory containing the disk images is located. Also ensure the interface matches the interface provided suring the configuration step. Once this is done you can set up the cluster.

```bash
sudo hctl bootstrap --mkfs ~/singlenode.yaml
```

To verify that the cluster is running and to get the correct information for the next steps, run 

```bash 
hctl status
```

You should get an output that looks like the following: 



Build hestia against motr, from the build directory: 

```bash
export MOTR_SRC_CODE=/path/to/cortx-motr/repo
cmake /path/to/hestia/repo -DHESTIA_WITH_MOTR=ON -DMOTR_SRC_DIR=$MOTR_SRC_CODE -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ 
make -j 4
```

Set environment variables 

```bash
export CLIENT_PROFILE="<0x7000000000000001:0x480>"    # profile id
export CLIENT_HA_ADDR="172.18.1.24@o2ib:12345:34:101" # ha-agent address
export CLIENT_LADDR="172.18.1.24@o2ib:12345:41:322"   # local address
export CLIENT_PROC_FID="<0x7200000000000001:0xdf>"    # process id

m0composite "$CLIENT_LADDR" "$CLIENT_HA_ADDR" "$CLIENT_PROFILE" "$CLIENT_PROC_FID"
```