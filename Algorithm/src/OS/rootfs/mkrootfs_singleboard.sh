#!/bin/bash

# 文件名称：mkrootfs_singleboard.sh
# 功能：单板根文件系统镜像制作脚本
# 使用：
# 用root权限运行，ubuntu可用sudo ./mkrootfs_singleboard.sh

# 默认制作正式版本镜像文件
# 如制作调试版本(即网络启动)，使用命令：
# ./mkrootfs_singleboard.sh debug

# 临时挂载目录
TMP=tmp

# 文件系统指定目录
OUR_DIR=usr/local/signalway
DRIVER_DIR=lib/modules
RC_LOCAL_DIR=etc/rc.d
BOOT_DIR=etc/rc.d/rc3.d

# 应用程序文件
APP_FILES="App/LprApp App/HvDsp.out"
LOADER="App/loader"

# 驱动文件
SLW_DEV_FILE="Script/slw-dev_single.ko"
DRIVER_FILES="Script/cmemk.ko Script/DSPLink.ko"

# 脚本
ADD_FILES="Script/run.sh Script/dm.sh Script/reloadko.sh"
# 单板在此文件设置网口
RC_LOCAL="Script/rc.local"
BOOT_FILE="Script/S99hdc"

# 需删除的文件
DEL_FILES="usr/sbin/goaheadbin/ usr/sbin/ifplugd"

###############################################################

#### 使用帮助
if [ $# -eq 1 ] && [ $1 != "debug" ]; then
    echo "usage: "
    echo "release version:" $0
    echo "debug version:" $0 "debug"
    exit
fi

#### 检查必要的文件是否都存在
cd patch
if [ $# -eq 0 ] && [ ! -f $LOADER ]; then
    echo "Error!! Not found" `basename $LOADER`
    exit
fi
if [ $# -eq 1 ] && [ $1 = "debug" ]; then
    for i in $APP_FILES; do
        if [ ! -f $i ]; then
            echo "Error!! Not found" `basename $i`
            exit
        fi
    done
fi
cd ../

###############################################################

# (解压文件系统镜像，得到initrd.img)
cp -f initrd.signalway.img.gz initrd.img.gz
gunzip -f initrd.img.gz

# (挂载到临时目录，如不存在，则创建)
if [ ! -d $TMP ]; then mkdir -p $TMP; fi
sudo mount -o loop initrd.img $TMP

cd patch

# (将patch目录对应文件复制到文件系统中)
echo "copying files ..."
# (防止因为复制而丢失可执行属性)
chmod +x * -R
for i in $ADD_FILES; do
    sudo cp -fp $i ../$TMP/$OUR_DIR/`basename $i`
done

if [ $# -eq 0 ]; then
    sudo cp -fp $LOADER ../$TMP/$OUR_DIR/`basename $LOADER`
fi
# 复制驱动文件
for i in $DRIVER_FILES; do
    sudo cp -fp $i ../$TMP/$DRIVER_DIR/`basename $i`
done

cp -fp $SLW_DEV_FILE ../$TMP/$DRIVER_DIR/slw-dev.ko

cp -fp $RC_LOCAL ../$TMP/$RC_LOCAL_DIR/rc.local

cp -fp $BOOT_FILE ../$TMP/$BOOT_DIR/`basename $BOOT_FILE`

if [ $# -eq 1 ] && [ $1 = "debug" ]; then
    for i in $APP_FILES; do
        sudo cp -fp $i ../$TMP/$OUR_DIR/`basename $i`
    done
echo "debug version"
fi

cd ../

# (删除)
for i in $DEL_FILES; do
    sudo rm -rf $TMP/$i
done

# (防止卸载时出现设备忙现象)
sleep 1

# (卸载临时目录)
sudo umount $TMP

# (压缩，生成initrd.img.gz)
gzip -fv9 initrd.img
mv initrd.img.gz initrd.img.gz.6467s

# (删除临时目录)
rm -rf $TMP

echo "Job done!"
