#!/bin/sh
#
# ×¢£º

umount -l /opt/ipnc
ubidetach /dev/ubi_ctrl -d 3

umount -l /mnt/nand
ubidetach /dev/ubi_ctrl -d 1
sleep 1
