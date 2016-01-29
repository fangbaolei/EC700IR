#!/bin/bash

#
# 功能: 制作NET分区镜像文件。
# 
# 注意：mkfs.ubifs命令的-c参数需要手动计算。可参考 http://processors.wiki.ti.com/index.php/UBIFS_Support
# 请使用root权限执行本脚本！

TMPFILE=ubifs.img
CFG_FILE=ubinize_net.cfg

TMP_DIR=net_tmp

if [ $# != 1 ]; then
    echo "usage:" $0 "[net file dir]"
    exit
fi

chmod +x bin/* -R

TMP=$1
#IMAGE_FILE should be “ubifs_net.img”
IMG_FILE=ubifs_net.img

cd bin

#./mk_ubifs.sh $TMP ../ubi_rootfs_back.img 40 rootfs
mkdir -p $TMP_DIR

cp -a ../$TMP/* $TMP_DIR

#step 1:生成配置文件
echo "1. generating cfg file"

echo "[ubifs]" > $CFG_FILE
echo "mode=ubi" >> $CFG_FILE
echo "image="$TMPFILE >> $CFG_FILE
echo "vol_id=0" >> $CFG_FILE
echo "vol_size=81MiB" >> $CFG_FILE
# dynamic
echo "vol_type=static" >> $CFG_FILE
echo "vol_name=net" >> $CFG_FILE
echo "vol_flags=autoresize" >> $CFG_FILE

#step 2:生成ubi镜像
chmod +x mkfs.ubifs
chmod +x ubinize

echo "2. generating ubi file"
./mkfs.ubifs -r $TMP_DIR -m 2048 -e 126976 -c 668 -o $TMPFILE
./ubinize -o ../$IMG_FILE -m 2048 -p 128KiB -s 2048 -O 2048 $CFG_FILE

#step 3:删除临时文件
echo "3.removing tmp files..."
sudo rm -rf $TMPFILE
sudo rm -rf $CFG_FILE
sudo rm -rf $TMP_DIR

echo ""
echo "!!!! generate img file: " $IMG_FILE "!!!!"
echo ""

cd ../
