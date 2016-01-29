#!/bin/bash

#
# 功能: 制作备份专用APP分区镜像文件。
# 
# 注意：mkfs.ubifs命令的-c参数需要手动计算。可参考 http://processors.wiki.ti.com/index.php/UBIFS_Support
# 请使用root权限执行本脚本！

OUTPUT_IMAGE=ubifs_app_back.img
FACTORY_IMAGE=ubifs_app_bak.factoryuse.img
TMPFILE=ubifs.img
CFG_FILE=ubinize_app.cfg

TMP_DIR=app_back_tmp

#应用程序目录
APP_DIR=$TMP_DIR

#图像采集程序目录
VIDEO_APP_DIR=$TMP_DIR/ipc_arch

TMP=backup_sys_app

#必要的文件
APP_FILES="backup_sys_app/deamon backup_sys_app/log backup_sys_app/command backup_sys_app/UpgradeApp backup_sys_app/command.sh backup_sys_app/fpga.xsvf backup_sys_app/fpga_178.xsvf backup_sys_app/fpga_185.xsvf backup_sys_app/fpga_249.xsvf backup_sys_app/load_fpga backup_sys_app/switchsys"
XML_FILES="backup_sys_app/deamon.xml"
INIT_FILE="backup_sys_app/my_run.sh"

APP_FILES1="../backup_sys_app/deamon ../backup_sys_app/log ../backup_sys_app/command ../backup_sys_app/UpgradeApp ../backup_sys_app/command.sh ../backup_sys_app/fpga.xsvf ../backup_sys_app/fpga_178.xsvf ../backup_sys_app/fpga_185.xsvf ../backup_sys_app/fpga_249.xsvf ../backup_sys_app/load_fpga ../backup_sys_app/switchsys"
XML_FILES1="../backup_sys_app/deamon.xml"
INIT_FILE1="../backup_sys_app/my_run.sh"

#判断必要的文件是否存在。若不存在则制作失败。
for i in $APP_FILES $XML_FILES $INIT_FILE; do
    if [ ! -f $i ]; 
	then
        echo -e "Error!! file not found:" $i
        exit
    fi
done

chmod +x bin/* -R

chmod +x $TMP/* -R

cd bin

mkdir -p $TMP_DIR

# 所有文件都添加可执行属性
chmod +x ../backup_sys_app/* -R

if [ ! -d "$APP_DIR" ]; then
mkdir -p $APP_DIR
fi
if [ ! -d "$VIDEO_APP_DIR" ]; then
mkdir -p $VIDEO_APP_DIR
fi


# 复制采集图像程序
cp -a ../backup_sys_app/video_test/* $VIDEO_APP_DIR

# 复制启动脚本
cp -a $INIT_FILE1 $APP_DIR

# 删除.svn目录
#find $APP_DIR -type d -name ".svn"|xargs rm -rf
#find $VIDEO_APP_DIR -type d -name ".svn"|xargs rm -rf

# todo
#cp -a ../backup_sys_app/load_fpga $VIDEO_APP_DIR
#cp -a ../backup_sys_app/fpga.xsvf $VIDEO_APP_DIR

# 必要的app程序
for i in $APP_FILES1 $XML_FILES1; do
    sudo cp -fp $i $APP_DIR/`basename $i`;
done

#cp -a ../$TMP/* $TMP_DIR

# 删除.svn目录
find $TMP_DIR -type d -name ".svn"|xargs rm -rf

#./mk_ubifs.sh $TMP ../ubi_rootfs_back.img 40 rootfs

#step 1:生成配置文件 
echo "1. generating cfg file"

echo "[ubifs]" > $CFG_FILE
echo "mode=ubi" >> $CFG_FILE
echo "image="$TMPFILE >> $CFG_FILE
echo "vol_id=0" >> $CFG_FILE
echo "vol_size=50MiB" >> $CFG_FILE
# dynamic static
echo "vol_type=static" >> $CFG_FILE
echo "vol_name=app_back" >> $CFG_FILE
echo "vol_flags=autoresize" >> $CFG_FILE

#step 2:生成ubi镜像
chmod +x mkfs.ubifs
chmod +x ubinize

#-c 471 was the result of calculation
echo "2. generating ubi file"

./mkfs.ubifs -r $TMP_DIR -m 2048 -e 126976 -c 416 -o $TMPFILE
./ubinize -o ../$OUTPUT_IMAGE -m 2048 -p 128KiB -s 2048 -O 2048 $CFG_FILE

# 工厂烧写(uboot烧写)专用
cp -f $TMPFILE ../$FACTORY_IMAGE

#step 3:删除临时文件
echo "3. removing tmp files..."
sudo rm -rf $TMPFILE
sudo rm -rf $CFG_FILE
sudo rm -rf $TMP_DIR

echo ""
echo "!!!! generate img file: " $OUTPUT_IMAGE "!!!!"
echo ""



cd ../
