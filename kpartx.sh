#!/bin/sh
sudo mkfs.hfsplus -v Hellorld bin/hellorld.dmg
LOOP=$(sudo kpartx -s -a -v bin/hellorld.dmg | awk -F'[ ]' '{print $3}' | tail -n1 )
sudo mount -o loop /dev/mapper/$LOOP ./dmg_mount/
