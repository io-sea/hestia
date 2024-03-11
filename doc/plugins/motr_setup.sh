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
export CLIENT_PROC_FID=$(hctl status | awk '/m0_client/ {print $3}' | head -n 1)
export CLIENT_LADDR=$(hctl status | awk '/m0_client/ {print $4}' | head -n 1)
# add angle brakcets to the ones of these that need them

m0composite "$CLIENT_LADDR" "$CLIENT_HA_ADDR" "$CLIENT_PROFILE" "$CLIENT_PROC_FID"

hctl status | sed '1,/Profile/!d' | awk '/pool/ {print}' | tail -n +2


