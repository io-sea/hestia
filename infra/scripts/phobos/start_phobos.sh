#!/bin/sh

# Arg 1 is phobos.conf location
cp $1 /etc/phobos.conf

# Set phobos flags
export PHOBOS_STORE_layout=raid1
export PHOBOS_LAYOUT_RAID1_repl_count=1
export DAEMON_PID_FILEPATH="/home/phobosd.pid"
export PHOBOS_LRS_lock_file="/home/phobosd.lock"

phobosd