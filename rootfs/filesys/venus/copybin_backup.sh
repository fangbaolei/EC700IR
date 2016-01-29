#!/bin/bash
#########################################################
# 本脚本用于拷贝编译生成的程序更新到做包目录
# 方便拷贝到同时，防止遗漏
#########################################################

#if err exit shell
set -e
  
#####################################################################
MAJOR_VERSION=3.1.0
MINOR_VERSION=1000	#default
  
MAIN_PATH=../../../ 
  
XML_CONFIG_PATH=${MAIN_PATH}/Config/
START_TIME=$(date +%s)
CONFIG_H=${MAIN_PATH}config.h

if [ -e $CONFIG_H ];then
	MINOR_VERSION=`cat $CONFIG_H | 
					grep "APP_VERSION" | 
					awk '{print $3}' | 
					tr -d '\n\r'`
fi

if [ -e $CONFIG_H ];then
	ROOTFS_VERSION=`cat $CONFIG_H | 
					grep "ROOTFS_VERSION" | 
					awk '{print $3}' | 
					tr -d '\n\r'`
fi

if [ "$MINOR_VERSION"x = x ];then
	MINOR_VERSION=1000
fi

HARDWARE_NAME='PCC200(PCC600)一体机'
HARDWARE_NAME_LITE='PCC200(PCC600)'
APP_VERSION=${MAJOR_VERSION}.${MINOR_VERSION}

IMAGE_NAME="BACKUP_${HARDWARE_NAME}_${APP_VERSION}"
   
SHELL_FILE=${XML_CONFIG_PATH}command.sh

APP_PATH=./backup_sys_app/
COMPILE_BIN_PATH=${MAIN_PATH}/bin/ARM/
 
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy arm bin to make image path!!!!"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
cp -vf ${COMPILE_BIN_PATH}UpgradeApp ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}command ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}deamon ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}log ${APP_PATH}
   
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy shell file to make image path"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
cp -vf ${SHELL_FILE} ${APP_PATH}
 

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "do something you like!!!!"
echo "and now I make app upgrade packet auto!"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
sudo ./mkubi_app_back.sh 
sudo ./mkrootfs_venus_back.sh 
./mk_venus_backsys.sh
cp ./venus_bak.img ${MAIN_PATH}/$IMAGE_NAME.img

PRODUCT_EXPORT_PATH=./product_export
if [ ! -d $PRODUCT_EXPORT_PATH ]; then mkdir -p $PRODUCT_EXPORT_PATH; fi
cp  -vf ../venus/0_uboot_kernel/u-boot.min.bin ${PRODUCT_EXPORT_PATH}/${HARDWARE_NAME_LITE}_u-boot_r.min.bin
cp  -vf ../venus/0_uboot_kernel/u-boot.bin ${PRODUCT_EXPORT_PATH}/${HARDWARE_NAME_LITE}_u-boot_r.bin
cp  -vf ../venus/0_uboot_kernel/uImage.venus ${PRODUCT_EXPORT_PATH}/${HARDWARE_NAME_LITE}_uImage_r${ROOTFS_VERSION}.venus
cp  -vf ./venus_rootfs_back.img ${PRODUCT_EXPORT_PATH}/${HARDWARE_NAME_LITE}_rootfs_back_r${ROOTFS_VERSION}.img  
cp  -vf ./ubifs_app_bak.factoryuse.img ${PRODUCT_EXPORT_PATH}/${HARDWARE_NAME_LITE}_ubifs_app_bak.factoryuse_r${MINOR_VERSION}.img

END_TIME=$(date +%s)
INTERVAL_TIME=$(($END_TIME - $START_TIME))
#echo -e '\033[0;33;1myellow\033[0m'
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "make upgrade image success, spend time: ${INTERVAL_TIME} s."
printf "application version: \e[0;32;1m$APP_VERSION\e[0m\n"
printf "upgrade image: \e[0;32;1m${IMAGE_NAME}.img\e[0m\n"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'

