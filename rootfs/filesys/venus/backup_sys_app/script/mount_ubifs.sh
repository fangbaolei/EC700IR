#!/bin/sh
#
# 使用：挂载备份应用层分区、DATA分区
# 注：此处根据已规划好的flash分区来挂载

# backup app
ubiattach /dev/ubi_ctrl -m 5 -O 2048 -d 1
ret=$?
if [ $ret -eq 0 ]; then
    sync
    sleep 1
    mount -r -o chk_data_crc -t ubifs /dev/ubi1_0 /opt/ipnc
fi

sleep 1
# 挂载data分区，单独使用程序挂载
smart_mount /dev/mtd10 /mnt/nand 3 data 2048

sync
