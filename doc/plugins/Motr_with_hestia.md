Download motr rpms from hestia repo, under Deploy > Package registry. Set an environement variable for their location as 

Note: If you have not yet installd the hestia base dependencies, do so before installing the rpms.

To install motr from the rpms provided in the hestia gitlab package registry, download the tarball from the repository and expand it, then run the following script (found in infra/scripts), ensuring that the argument passed in is the path to the top level 'builds' folder

```bash 
    install_motr_rpms.sh path/to/builds/folder    
```

In order to build hestia alongside motr, it is neccessary to clone the motr source code from github. Hestia must be pointed to the location of the motr source code when compiling

```bash 
    git clone https://github.com/Seagate/cortx-motr.git
```

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