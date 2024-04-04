#!/bin/sh

# Start postgres
su postgres -c "postgres -D /home/postgres/data&"
sleep 5s

# Start phobos
export PHOBOS_STORE_layout=raid1
export PHOBOS_LAYOUT_RAID1_repl_count=1
export DAEMON_PID_FILEPATH="/home/phobosd.pid"
export PHOBOS_LRS_lock_file="/home/phobosd.lock"
#rm $DAEMON_PID_FILEPATH
#rm $PHOBOS_LRS_lock_file
#mkdir -p /home/phobos_test_dir
phobosd
phobos dir add /phobos_mount
phobos dir format --fs posix --unlock /phobos_mount
sleep 5s

# Start hestia
hestia server --config /configs/hestia.yaml