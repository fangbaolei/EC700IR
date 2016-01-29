#!/bin/bash

# 文件名称：mkrootfs_mercury.sh
# 功能：水星平台文件系统镜像制作脚本
# 作者：lijj 2013-04

# 修改须知：参考脚本的方法自行添加、删除文件

# 使用：
# ！！！必须使用root权限运行！！！
# 正式版本镜像文件：
# ./mkrootfs_mercury.sh
# 调试版本(即网络启动)
# ./mkrootfs_mercury.sh debug

##########################################################

######################## 可修改项 开始

# 原根文件系统名称
ORG_ROOTFS="rootfs_dm6467.32MB.img.gz"
# 新根文件系统名称
NEW_ROOTFS="rootfs.img.gz.mercury"

# 文件系统指定目录
OUR_DIR=usr/local/signalway
DRIVER_DIR=lib/modules
BOOT_DIR=etc/rc.d/rc3.d
BIN_DIR=usr/sbin
ETC_DIR=etc
LIB_DIR=usr/lib

# todo：确定好文件名称，修改
# !!!!!注意！！请根据实际情况修改以下各项！！！！！！！！

# ARM与DSP程序、FPGA文件
ARM_DSP_FILES="App/LprApp App/MercuryDsp.out App/fpga.xsvf"
# 应用程序文件
APP_FILES=" App/deamon App/command App/log App/fpga_test App/UpgradeApp App/OnvifApp App/RtspApp"
XML_FILES="App/deamon.xml App/LprApp.xml App/command.sh App/fz_songti.ttf"
LOADER="App/loader"

# 由于DSP程序体积过大，在脚本中进行精简
HvCoffReduce="Script/HvCoffReduce"
COFF_DSP="App/MercuryDsp.out"
COFF_TMP="App/HvDsp_tmp.out"

# 驱动文件
SLW_DEV_FILE="Script/slw-dev_mercury.ko"
DRIVER_FILES="Script/cmemk.ko Script/DSPLink.ko Script/musb_hdrc.ko Script/usb-storage.ko"

# 脚本
ADD_FILES="Script/run.sh Script/dm.sh Script/reloadko.sh"
BOOT_FILE="Script/S99hdc"

BIN_FILES="Script/ntpdate"
ETC_FILES="Script/localtime"
LIB_FILES="Script/libcap.so.1 Script/libelf.so.1 Script/libmd5.so.0"

# 需删除的文件
DEL_FILES=

######################## 可修改项 结束

##########################################
# 颜色及提示信息
COLOR_G="\x1b[0;32m" # green
COLOR_R="\x1b[1;31m" # red
COLOR_C="\x1b[1;35m"
RESET="\x1b[0m"

STR_ERR="[Oops!! Error occurred!! Please see the message upside!!]"
STR_OK="[Job done!]"
STR_FILE="New rootfs: "$NEW_ROOTFS

MSG_ERR=$COLOR_R$STR_ERR$RESET
MSG_OK=$COLOR_G$STR_OK$RESET
MSG_FILE=$COLOR_C$STR_FILE$RESET
###########################################

# 临时挂载目录
TMP=$PWD/tmp

###############################################################

myexit()
{
if [ $1 -eq "1" ]
then
    # 前面已经挂载，这里要卸载
    if [ $2 ]
    then
        #echo "umounting" $TMP "..."
        sleep 1
        sudo umount $TMP
        rm -rf $TMP
    fi
    echo -e $MSG_ERR
    exit;
fi
}

#### 使用帮助
if [ $# -eq 1 ] && [ $1 != "debug" ]; then
    echo "usage: "
    echo "release version:" $0
    echo "debug version:" $0 "debug"
    exit
fi

#### 检查必要的文件是否都存在
cd patch

for i in $APP_FILES $XML_FILES $LOADER; do
    if [ ! -f $i ]; then
        echo -e "Error!! Not found" `basename $i`
        exit
    fi
done

if [ $# -eq 1 ] && [ $1 = "debug" ]; then
    for i in $ARM_DSP_FILES; do
        if [ ! -f $i ]; then
            echo -e "Error!! Not found" `basename $i`
            exit
        fi
    done
fi
cd ../

###############################################################

# (解压文件系统镜像，得到initrd.img)
cp -f  $ORG_ROOTFS initrd.img.gz
ret=$?
myexit $ret

gunzip -f initrd.img.gz

# (挂载到临时目录，如不存在，则创建)
if [ ! -d $TMP ]; then mkdir -p $TMP; fi
sudo mount -o loop initrd.img $TMP

cd patch
ret=$?
myexit $ret 1

# (将patch目录对应文件复制到文件系统中)
echo "copying files ..."
# (防止因为复制而丢失可执行属性)
chmod +x * -R

ret=$?
myexit $ret 1

# 必要的文件
for i in $ADD_FILES $LOADER; do
    sudo cp -fp $i $TMP/$OUR_DIR/`basename $i`;
    ret=$?
    myexit $ret 1
done


# 复制驱动文件
for i in $DRIVER_FILES; do
    sudo cp -fp $i $TMP/$DRIVER_DIR/`basename $i`
    ret=$?
    myexit $ret 1
done

cp -fp $SLW_DEV_FILE $TMP/$DRIVER_DIR/slw-dev.ko
ret=$?
myexit $ret 1

cp -fp $BOOT_FILE $TMP/$BOOT_DIR/`basename $BOOT_FILE`
ret=$?
myexit $ret 1

for i in $BIN_FILES; do
    sudo cp -fp $i $TMP/$BIN_DIR/`basename $i`
    ret=$?
    myexit $ret 1
done

for i in $ETC_FILES; do
    sudo cp -fp $i $TMP/$ETC_DIR/`basename $i`
    ret=$?
    myexit $ret 1
done

for i in $LIB_FILES; do
    sudo cp -fp $i $TMP/$LIB_DIR/`basename $i`
    ret=$?
    myexit $ret 1
done

for i in $APP_FILES $XML_FILES; do
    sudo cp -fp $i $TMP/$OUR_DIR/`basename $i`
    ret=$?
    myexit $ret 1
done

# 复制应用程序(网络启动情况下)
if [ $# -eq 1 ] && [ $1 = "debug" ]; then

    ## 精简DSP程序
    $HvCoffReduce $COFF_DSP $COFF_TMP
    sudo mv $COFF_TMP $COFF_DSP
    ret=$?
    myexit $ret 1

    for i in $ARM_DSP_FILES; do
        sudo cp -fp $i $TMP/$OUR_DIR/`basename $i`
        ret=$?
        myexit $ret 1
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
mv initrd.img.gz $NEW_ROOTFS

# (删除临目录)
rm -rf $TMP

echo -e $MSG_OK
echo -e $MSG_FILE
