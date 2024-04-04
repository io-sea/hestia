#!/bin/sh
# Set up db
useradd postgres
mkdir /home/postgres
chown postgres /home/postgres

su postgres -c "initdb -D /home/postgres/data"

# Remove some db restrictions to allow access from podman containers
echo "listen_addresses = '*'" >> /home/postgres/data/postgresql.conf
sed -i -e 's|127.0.0.1/32|all|' /home/postgres/data/pg_hba.conf

su postgres -c "postgres -D /home/postgres/data&"
su postgres -c "python3 -m phobos.db setup_db -s -p phobos"

# Setup phobos deamon
cp phobos.conf /etc/
#export PHOBOS_STORE_layout=raid1
#export PHOBOS_LAYOUT_RAID1_repl_count=1
#export DAEMON_PID_FILEPATH="/home/phobosd.pid"
#export PHOBOS_LRS_lock_file="/home/phobosd.lock"
#phobosd

# Sanity check by creating a directory
#rm -rf $2
#mkdir -p $2
#phobos dir add $2
#phobos dir format --fs posix --unlock $2
#phobos dir update --tags $3 $2
