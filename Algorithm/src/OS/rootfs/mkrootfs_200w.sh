#!/bin/bash

# 文件名称：mkrootfs_200w.sh
# 功能：200w一体机根文件系统镜像制作脚本
# 使用：
# 用root权限运行，ubuntu可用sudo ./mkrootfs_200w.sh

# 默认制作正式版本镜像文件
# 如制作调试版本(即网络启动)，使用命令：
# ./mkrootfs_200w.sh debug

# 临时挂载目录
TMP=tmp

# 文件系统指定目录
OUR_DIR=usr/local/signalway
DRIVER_DIR=lib/modules
BOOT_DIR=etc/rc.d/rc3.d

# 主从应用程序文件
APP_MASTER="App/CamApp App/CamDsp.out App/fpga.xsvf"
APP_SLAVE="App/LprApp App/HvDsp.out"
LOADER="App/loader"

# 驱动文件
SLW_DEV_FILE="Script/slw-dev_200w.ko"
DRIVER_FILES="Script/cmemk.ko Script/DSPLink.ko  \
              Script/slw-pci.slave.ko Script/slw-pci.master.ko"
# 脚本
ADD_FILES="Script/run.sh Script/dm.sh Script/reloadko.sh"
ADD_KO_MASTER="Script/loadko_pci_master.sh"
ADD_KO_SLAVE="Script/loadko_pci_slave_200w.sh"
BOOT_FILE="Script/S99hdc"

# 主端暂时屏蔽WEB
DEL_FILES_M="usr/sbin/goaheadbin/"

# 从端无网络，删除相关的文件
DEL_FILES="usr/sbin/goaheadbin/ usr/sbin/ifplugd"

# 由于DSP程序体积过大，在脚本中进行精简(针对网络启动版本)
HvCoffReduce="Script/HvCoffReduce"
COFF_DSP1="App/CamDsp.out"
COFF_TMP1="App/CamDsp_tmp.out"
COFF_DSP2="App/HvDsp.out"
COFF_TMP2="App/HvDsp_tmp.out"

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
if [ ! -f $LOADER ]; then
    echo "Error!! Not found" `basename $LOADER`
    exit
fi
if [ $# -eq 1 ] && [ $1 = "debug" ]; then
    for i in $APP_MASTER $APP_SLAVE; do
        if [ ! -f $i ]; then
            echo "Error!! Not found" `basename $i`
            exit
        fi
    done
fi
cd ../

#################################################主端

# (解压文件系统镜像，得到initrd.img)
cp -f initrd.signalway.img.gz initrd.img.gz
gunzip -f initrd.img.gz

# (挂载到临时目录，如不存在，则创建)
if [ ! -d $TMP ]; then mkdir -p $TMP; fi
sudo mount -o loop initrd.img $TMP

cd patch

# (将patch目录对应文件复制到文件系统中)
echo "copying master files ..."
chmod +x * -R
for i in $LOADER $ADD_FILES $ADD_KO_MASTER; do
    sudo cp -fp $i ../$TMP/$OUR_DIR/`basename $i`
done

# 复制驱动文件
for i in $DRIVER_FILES; do
    sudo cp -fp $i ../$TMP/$DRIVER_DIR/`basename $i`
done

cp -fp $SLW_DEV_FILE ../$TMP/$DRIVER_DIR/slw-dev.ko

cp -fp $BOOT_FILE ../$TMP/$BOOT_DIR/`basename $BOOT_FILE`

if [ $# -eq 1 ] && [ $1 = "debug" ]; then

    ## 精简DSP程序
    $HvCoffReduce $COFF_DSP1 $COFF_TMP1
    sudo mv $COFF_TMP1 $COFF_DSP1

    for i in $APP_MASTER; do
        sudo cp -fp $i ../$TMP/$OUR_DIR/`basename $i`
    done
    echo "master: debug version"
fi

cd ../

####

mv $TMP/$OUR_DIR/`basename $ADD_KO_MASTER` \
   $TMP/$OUR_DIR/loadko.sh

# (删除)
for i in $DEL_FILES_M; do
    sudo rm -rf $TMP/$i
done

# (防止卸载时出现设备忙现象)
sleep 1

# (卸载临时目录)
sudo umount $TMP

# (压缩，生成initrd.img.gz)
gzip -fv9 initrd.img
mv initrd.img.gz initrd.img.gz.master

#################################################从端

# (解压文件系统镜像，得到initrd.img)
cp -f initrd.signalway.img.gz initrd.img.gz
gunzip -f initrd.img.gz

# (挂载到临时目录，如不存在，则创建)
if [ ! -d $TMP ]; then mkdir -p $TMP; fi
sudo mount -o loop initrd.img $TMP

cd patch

# (将patch目录对应文件复制到文件系统中)
echo "copying slave files ..."
chmod +x * -R
for i in $LOADER $ADD_FILES $ADD_KO_SLAVE; do
    sudo cp -fp $i ../$TMP/$OUR_DIR/`basename $i`
done

# 复制驱动文件
for i in $DRIVER_FILES; do
    sudo cp -fp $i ../$TMP/$DRIVER_DIR/`basename $i`
done

cp -fp $SLW_DEV_FILE ../$TMP/$DRIVER_DIR/slw-dev.ko

if [ $# -eq 0 ]; then
    cp -fp $BOOT_FILE ../$TMP/$BOOT_DIR/`basename $BOOT_FILE`
fi

if [ $# -eq 1 ] && [ $1 = "debug" ]; then

    ## 精简DSP程序
    $HvCoffReduce $COFF_DSP2 $COFF_TMP2
    sudo mv $COFF_TMP2 $COFF_DSP2

    for i in $APP_SLAVE; do
        sudo cp -fp $i ../$TMP/$OUR_DIR/`basename $i`
    done
    rm -rf ../$TMP/$OUR_DIR/`basename $LOADER`
    echo "slave: debug version"
fi

cd ../

mv $TMP/$OUR_DIR/`basename $ADD_KO_SLAVE` \
   $TMP/$OUR_DIR/loadko.sh

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
mv initrd.img.gz initrd.img.gz.slave

# (删除临时目录)
rm -rf $TMP

echo "Job done!"
