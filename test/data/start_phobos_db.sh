#!/bin/sh

script="${BASH_SOURCE[0]:-${(%):-%x}}"
script_dir="$( cd "$( dirname "${script}" )" >/dev/null 2>&1 && pwd )"

set -x

# Test root to start test directory tree
test_dir=/home

cp ${script_dir}/phobos.conf /etc/

# create test directories
export PHOBOS_STORE_layout=raid1
export PHOBOS_LAYOUT_RAID1_repl_count=1

export PYTHONPATH=/phobos/src/cli/build/lib.linux-$(arch)-3.6
# Initialize phobos db
useradd postgres
mkdir /home/postgres
chown postgres /home/postgres
su postgres -c "initdb -D /home/postgres/data"
su postgres -c "postgres -D /home/postgres/data&"
su postgres -c "PYTHONPATH=${PYTHONPATH} python3 -m phobos.db setup_db -s -p phobos"

export PHOBOSD_PID_FILEPATH="$test_dir/phobosd.pid"
export PHOBOS_LRS_lock_file="$test_dir/phobosd.lock"

phobosd
test_phobos_dir=${test_dir}/test-phobos-dir
rm -rf ${test_phobos_dir}
mkdir -p ${test_phobos_dir}
# chmod 777 -R ${test_phobos_dir} # for debug as non-root


phobos=/phobos/src/cli/build/scripts-3.6/phobos
$phobos dir add ${test_phobos_dir}
$phobos dir format --fs posix --unlock ${test_phobos_dir}
$phobos dir update --tags test_phobos_dir ${test_phobos_dir}
set +x

# Stop:
# export PHOBOSD_PID_FILEPATH="$test_dir/phobosd.pid"
# PID_DAEMON=`cat $PHOBOSD_PID_FILEPATH`
# if [ $PID_DAEMON -eq 0 ]; then
#     return 0
# fi

# kill $PID_DAEMON &>/dev/null || echo "Daemon was not running"
# # wait would not work here because PID_DAEMON is not an actual child
# # of this shell (created by phobosd in invoke_daemon)
# tail --pid=$PID_DAEMON -f /dev/null
# PID_DAEMON=0