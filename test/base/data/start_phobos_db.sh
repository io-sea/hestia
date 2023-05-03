#!/bin/sh

script="${BASH_SOURCE[0]:-${(%):-%x}}"
script_dir="$( cd "$( dirname "${script}" )" >/dev/null 2>&1 && pwd )"

localectl set-locale LANG=en_US.UTF-8
export PATH=$PATH:/usr/pgsql-9.4/bin/

set -x

# Test root to start test directory tree
test_dir=/home

cp ${script_dir}/phobos.conf /etc/

# create test directories and database
test_db_dir=${test_dir}/test-db
rm -rf "${test_db_dir}"
mkdir -p "${test_db_dir}"
chown -R postgres:postgres "${test_db_dir}"
chmod 0700 "${test_db_dir}"

su postgres -c "initdb ${test_db_dir}"

# Start Phobos database
su postgres -c "pg_ctl -D ${test_db_dir} start"

export PHOBOS_STORE_layout=raid1
export PHOBOS_LAYOUT_RAID1_repl_count=1

# Initialize phobos db
su postgres -c "python3 -m phobos.db setup_db -s -p phobos"

mkdir -p /run/phobosd

phobosd

test_phobos_dir=${test_dir}/test-phobos-dir
rm -rf ${test_phobos_dir}
mkdir -p ${test_phobos_dir}
phobos dir add --unlock ${test_phobos_dir}
phobos dir format --fs posix ${test_phobos_dir}

set +x


# Stop:
# su postgres -c "pg_ctl -D /home/test-db stop"
# rm -rf /run/phobosd