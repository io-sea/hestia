# Running Phobos in Parallel

These are some rough notes for doing this with containers.

Ultimately this needs the Phobos postgres db to be exposed from a container, this requires some changes in the postgres configs and also giving the address of the db a 'hostname' in any 'child' container /etc/hosts. 

It is useful to use pgadmin on the host (assuming Mac) to make sure everything is set up ok also.

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
PGPASSWORD=password psql -h localhost -p 5432 -U username
```

to connect.

To find the host address on Mac you can do:

```sh
ifconfig 
>>>
...
en0: flags=8863<UP,BROADCAST,SMART,RUNNING,SIMPLEX,MULTICAST> mtu 1500
...
```

Something like above will have an `inet` address.

The host address needs to be added to `/etc/hosts` in any container connecting to the phobosdb, something like:

```
192.168.11.48:8000 phobos_db_node
```

that needs to be done when building the container.

When launching the container you can point the db port to something in the non-root range, like 8080:

```sh
podman run -it --platform linux/arm64 -p 8080:5432 -v $HOME:$HOME phobos_build
```

On the host running the phobos db we need to add `listen_addresses = '*'` to `/home/postgres/data/postgresql.conf` and modify `/home/postgres/data/pg_hba.conf` to have `all` for ipv4 addresses:

```
# IPv4 local connections:
host    all             all             all            trust
```

Then restart the server:

```sh
su postgres -c "pg_ctl stop -D /home/postgres/data"
su postgres -c "pg_ctl start -D /home/postgres/data"
```

The `/etc/phobos.conf` on containers connecting to the db need to have the hostname, like phobos_db_node set above, like:

```
connect_string = dbname=phobos host=phobos_db_node user=phobos password=phobos
```

Now you can create some devices or objects on the nodes.
