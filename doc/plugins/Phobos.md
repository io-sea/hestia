# Running Phobos in Parallel

Phobos can be run in a multi-node configuration using containers. On will run with the phobos db with a registered 'dir' type drive and the other as a 'worker', just with a drive but no db.

## Host Setup

### DB Viewer

It is useful to use pgadmin db viewer on the host (assuming Mac) to make sure everything is set up ok with the phobos db.

```sh
brew install --cask pgadmin4
```

To run the UI you can do:

```sh
/Applications/pgAdmin\ 4.app/Contents/MacOS/pgAdmin\ 4
```

You can also install libpqsl with brew, it will give you a path to set and you can do:

```sh
brew install libpq
export PATH="/opt/homebrew/opt/libpq/bin:$PATH"
PGPASSWORD=phobos psql -h localhost -p 5432 -U phobos
```

to connect.

### Host IP Address

We need to find the host ip address, on Mac you can do:

```sh
ifconfig 
>>>
...
en0: flags=8863<UP,BROADCAST,SMART,RUNNING,SIMPLEX,MULTICAST> mtu 1500
...
```

Something like above will have an `inet` address.

## Build containers

We will build two containers, first for the node with the db:

```sh
mkdir phobos_db_node
cd phobos_db_node
cp $HESTIA_SRC_DIR/infra/scripts/phobos/* .
cp $HESTIA_SRC_DIR/infra/scripts/phobos/db_node/Dockerfile .
podman build --platform linux/arm64 -t phobos_db_node .
```

Then the node which will be the worker:

```sh
mkdir phobos_worker_node
cd phobos_worker_node
cp $HESTIA_SRC_DIR/infra/scripts/phobos/* .
cp $HESTIA_SRC_DIR/infra/phobos/scripts/worker_node/Dockerfile .
podman build --platform linux/arm64 -t phobos_worker_node .
```

## Running the containers

We can launch the container with the db with:

```sh
podman run -it --platform linux/arm64 -p 5432:5432 -p 8080:8080 -p 8090:8090 -v $HOME:$HOME -h phobos_db_node phobos_db_node
```

Inside the container start the db, launch phobos, create a storage device and add an object:

```sh
su postgres -c "pg_ctl start -D /home/postgres/data"
./start_phobos.sh phobos.conf
./phobos_add_dir.sh /home/phobos_db_device phobos_db_device
export PYTHONPATH=/phobos/src/cli/build/lib.linux-$(arch)-3.6
alias phobos=/phobos/src/cli/build/scripts-3.6/phobos
phobos put --family dir phobos.conf phobos_db_file
```

See if you can access the db from the host now with pgadmin or libpq.

We can launch the worker with:

```sh
podman run -it --platform linux/arm64 -v $HOME:$HOME -h phobos_worker_node0 
--add-host phobos_db_host:<my_host_ip> --network=host phobos_worker_node
```

Inside the worker container start phobos and create a storage device:

```sh
./start_phobos.sh phobos.conf
./phobos_add_dir.sh /home/phobos_worker_device0 phobos_worker_device0
export PYTHONPATH=/phobos/src/cli/build/lib.linux-$(arch)-3.6
alias phobos=/phobos/src/cli/build/scripts-3.6/phobos
phobos put --family dir phobos.conf phobos_worker_file
```

# Running a 'phobos locate' demo system

To run a system that can demo Phobos locate we can launch a node with a db (Controller) and two worker nodes. The configs [here](/test/data/configs/phobos/deimos/) correspond to the nodes. You need to build Hestia in the containers and replace the Host ip address in the configs (currently `192.168.0.178` in there).

Then launch the controller and both workers, you should be able to use the Hestia s3 tool [here](/src/integrations/s3_main.cc) to put an object to the controller, it will end up on one of the workers. You can then try to get it from a different worker using the `preferred_node` flag - it should automatically redirect to the worker with the data and return the object data to the host.
