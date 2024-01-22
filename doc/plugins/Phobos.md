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
cp $HESTIA_SRC_DIR/src/infra/phobos/* .
cp $HESTIA_SRC_DIR/src/infra/phobos/db_node/Dockerfile .
podman build --platform linux/arm64 -t phobos_db_node .
```

Then the node which will be the worker:

```sh
mkdir phobos_worker_node
cd phobos_worker_node
cp $HESTIA_SRC_DIR/src/infra/phobos/* .
cp $HESTIA_SRC_DIR/src/infra/phobos/worker_node/Dockerfile .
cp $HESTIA_SRC_DIR/src/infra/phobos/worker_node/Dockerfile .
podman build --platform linux/arm64 -t phobos_worker_node .
```

## Running the containers

We can launch the container with the db with:

```sh
podman run -it --platform linux/arm64 -p 8080:5432 -v $HOME:$HOME -h phobos_db_node phobos_db_node
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
podman run -it --platform linux/arm64 -v $HOME:$HOME -h phobos_worker_node 
--add-host phobos_db_host:<my_host_ip> --network=host phobos_worker_node
```

Inside the worker container start phobos and create a storage device:

```sh
./start_phobos.sh phobos.conf
./phobos_add_dir.sh /home/phobos_worker_device phobos_worker_device
export PYTHONPATH=/phobos/src/cli/build/lib.linux-$(arch)-3.6
alias phobos=/phobos/src/cli/build/scripts-3.6/phobos
phobos put --family dir phobos.conf phobos_worker_file
```
