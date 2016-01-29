#!/bin/bash

# 功能：压缩镜像并挂载到指定目录。
# 注：本脚本属高级用法，不作出错处理。
#
# 使用：用root权限运行，或者sudo ./ungunzip.sh [压缩镜像文件] [挂载目录]
# 仅在ubuntu下测试通过

TMP1=tmp

RFS_TMP=rootfs_tmp.img.gz
RFS_1=rootfs_tmp.img

if [ $# -ne 2 ]; then
    echo "usage:" $0 "[rootfs img file] [rootfs dir]"
    exit
fi

cp -f $1 $RFS_TMP
gunzip -f $RFS_TMP

TMP=$2

# (挂载到临时目录，如不存在，则创建)
if [ ! -d $TMP1 ]; then mkdir -p $TMP1; fi
sudo mount -o loop $RFS_1 $TMP1

sudo cp -a $TMP1 $TMP

sudo umount $TMP1

sudo rm -rf $TMP1
sudo rm -rf $RFS_1

sudo rm -rf $TMP/lost+found

# 注：需要自行卸载
# 卸载
# umount $TMP