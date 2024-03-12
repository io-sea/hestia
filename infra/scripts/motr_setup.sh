#!/bin/bash 
dest="$HOME/var/motr"
mkdir -p "$dest" 
for i in {0..9}; do 
	dd if=/dev/zero of="$dest/disk$i.img" bs=1M seek=9999 count=1 
done

sudo hctl bootstrap --mkfs ~/singlenode-multipools.yaml
#pull this file into the hestia repo

export CLIENT_HA_ADDR=$(hctl status | awk '/hax/ {print $4}')
export CLIENT_PROFILE=$(hctl status | awk '/default/ {print $1}')
export CLIENT_PROFILE="<$CLIENT_PROFILE>"
export CLIENT_PROC_FID=$(hctl status | awk '/m0_client/ {print $3}' | head -n 1)
export CLIENT_PROC_FID="<$CLIENT_PROC_FID>"
export CLIENT_LADDR=$(hctl status | awk '/m0_client/ {print $4}' | head -n 1)
# add angle brakcets to the ones of these that need them

m0composite "$CLIENT_LADDR" "$CLIENT_HA_ADDR" "$CLIENT_PROFILE" "$CLIENT_PROC_FID"

export POOLS=$(hctl status | sed '1,/Profile/!d' | awk '/pool/ {print}' | tail -n +2 | awk '{print $1}')
export NUM_POOLS=$(echo $POOLS | wc -w)

echo "Add the following config in the motr backend section of your hestia config yaml file:"
echo   "root: motr_tests_object_store"
echo   "source: plugin"
echo   "type: hsm"
echo   "plugin_path: libhestia_motr_plugin"
echo   "ha_address: "$CLIENT_HA_ADDR
echo   "local_address: "$CLIENT_LADDR
echo   "proc_fid: "$CLIENT_PROC_FID
echo   "profile: "$CLIENT_PROFILE  
echo   "tier_info: "name=M0_POOL_TIER1,identifier=<0x6f00000000000001:0x0>;name=M0_POOL_TIER2,identifier=<0x6f00000000000001:0x1>;name=M0_POOL_TIER3,identifier=<0x6f00000000000001:0x2>"

export i=1
for pool in $POOLS
do
        echo "name=M0_POOL_TIER"$i",identifier="$pool
        ((i++))
done

