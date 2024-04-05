#!/bin/sh

useradd postgres
mkdir /home/postgres
chown postgres /home/postgres

su postgres -c "initdb -D /home/postgres/data"

# Remove some db restrictions to allow access from podman containers
echo "listen_addresses = '*'" >> /home/postgres/data/postgresql.conf
sed -i -e 's|127.0.0.1/32|all|' /home/postgres/data/pg_hba.conf

su postgres -c "postgres -D /home/postgres/data&"
su postgres -c "python3 -m phobos.db setup_db -s -p phobos"