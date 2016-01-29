#!/bin/sh
# 主加载PCI驱动(不区分200万、500万)

insmod /lib/modules/slw-pci.master.ko
rm -f /dev/swpci
mknod /dev/swpci c `awk "\\$2==\"swpci\" {print \\$1}" /proc/devices` 0
