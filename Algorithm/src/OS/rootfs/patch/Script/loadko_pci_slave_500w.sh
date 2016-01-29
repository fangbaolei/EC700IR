#!/bin/sh
# 从端加载PCI驱动
# 500万一体机

insmod /lib/modules/slw-pci.slave.ko rbuf_phys_start=0x86000000 rbuf_phys_end=0x86800000 wbuf_phys_start=0x86800000 wbuf_phys_end=0x87000000
rm -f /dev/swpci
mknod /dev/swpci c `awk "\\$2==\"swpci\" {print \\$1}" /proc/devices` 0
