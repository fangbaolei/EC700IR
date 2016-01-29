#!/bin/bash

# 功能：将文件系统目录做成镜像文件并压缩。
# 当前目录须存在ROOTFS目录，里面是根文件系统内容
# 注：请自行修改ROOTFS、ROOTIMG、SIZE的定义。
#
# 使用：用root权限运行，或者sudo ./mkrootfs.sh [文件系统目录] [压缩镜像文件] [镜像大小]
# 仅在ubuntu下测试通过

ROOTFS=filesys_org
TMP=tmp1
ROOTIMG=rootfs.dm8127.img
# 16: 16384
# 32: 32768
# 48: 49152
# 64: 65536
SIZE=49152

if [ $# -ne 3 ]; then
    echo "usage:" $0 "[rootfs dir] [rootfs img file] [size]"
    echo "eg:" $0 "rootfs_org rootfs.img(!!!not .img.gz!!!) 49152"
    exit
fi

ROOTFS=$1
ROOTIMG=$2
SIZE=$3

# (创建对应大小的映像文件)
dd if=/dev/zero of=$ROOTIMG bs=1k count=$SIZE
# (格式化)
mke2fs -F -v -m0 $ROOTIMG
# (挂载到同一目录的tmp)
if [ ! -d $TMP ]; then mkdir -p $TMP; fi
sudo mount -o loop $ROOTIMG $TMP
# (将rootfs目录下所有文件复制到tmp中)
echo "copying..."
sudo cp -a $ROOTFS/* $TMP
# (卸载tmp)
sudo umount $TMP
# (压缩之)
gzip -fv9 $ROOTIMG

#sudo umount $ROOTFS
#rm -rf $ROOTFS

# (删除临目录)
rm -rf $TMP
