#!/bin/bash

#
# 功能: 制作APP分区镜像文件。
# 
# 注意：mkfs.ubifs命令的-c参数需要手动计算。可参考 http://processors.wiki.ti.com/index.php/UBIFS_Support
# 请使用root权限执行本脚本！

# 部分文件与金星平台共用，用此宏定义
INPUT_DIR=../venus
OUTPUT_DIR=$PWD

IMG_FILE=ubifs_app.img
TMPFILE=ubifs.img
CFG_FILE=ubinize_app.cfg
DSPAPP=bin/ipnc_rdk_fw_c6xdsp.xe674
ARMAPP=LprApp

TMP_DIR=app_tmp

if [ $# != 1 ]; then
    echo "usage:" $0 "[app file dir]"
    exit
fi

chmod +x $INPUT_DIR/bin/* -R

TMP=$1

chmod +x $TMP/* -R

cd $INPUT_DIR/bin

mkdir -p $TMP_DIR

cp -a $OUTPUT_DIR/$TMP/* $TMP_DIR

# 删除.svn目录
find $TMP_DIR -type d -name ".svn"|xargs rm -rf

# 因为有空格，特殊处理
cd $TMP_DIR/web_publish/Application\ Files
find . -type d -name ".svn"|xargs rm -rf
cd -
###########　加密
cd $TMP_DIR
# ...
echo "Encrypt app file.............................."
../EncryptApp -f $ARMAPP -t 0 -e 1 -d ./
../EncryptApp -f $DSPAPP -t 1 -e 1 -d ./bin/
echo "Encrypt app file finish,remove raw file........"
#cp ipnc_rdk_fw_c6xdsp.xe674_e ../
#cp LprApp_e ../
rm -vf $ARMAPP $DSPAPP

cd -
#./mk_ubifs.sh $TMP ../ubi_rootfs_back.img 40 rootfs

#step 1:生成配置文件 
echo "1. generating cfg file"

echo "[ubifs]" > $CFG_FILE
echo "mode=ubi" >> $CFG_FILE
echo "image="$TMPFILE >> $CFG_FILE
echo "vol_id=0" >> $CFG_FILE
echo "vol_size=57MiB" >> $CFG_FILE
# dynamic static
echo "vol_type=static" >> $CFG_FILE
#echo "vol_type=dynamic" >> $CFG_FILE
echo "vol_name=app" >> $CFG_FILE
echo "vol_flags=autoresize" >> $CFG_FILE

#step 2:生成ubi镜像
chmod +x mkfs.ubifs
chmod +x ubinize

#-c 471 was the result of calculation
echo "2. generating ubi file"
# 减少一个PEB，原来是471
./mkfs.ubifs -r $TMP_DIR -m 2048 -e 126976 -c 470 -o $TMPFILE
./ubinize -o $OUTPUT_DIR/$IMG_FILE -m 2048 -p 128KiB -s 2048 -O 2048 $CFG_FILE

#step 3:删除临时文件
echo "3. removing tmp files..."
sudo rm -rf $TMPFILE
sudo rm -rf $CFG_FILE
sudo rm -rf $TMP_DIR

echo ""
echo "!!!! generate img file: " $IMG_FILE "!!!!"
echo ""


cd ../
