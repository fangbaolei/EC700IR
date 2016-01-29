#!/bin/bash

#
# 功能: 制作根文件系统镜像。
# 
# 请使用root权限执行本脚本！

#参数定义
OUTPUT_IMAGE=venus_rootfs.img
OUTPUT_IMGE_GZ=$OUTPUT_IMAGE.gz
ORG_FS_DIR=rootfs_org

# 原始根文件系统镜像
ROOTFS_IMG=rootfs.dm8127.img.gz

#临时专用
RFS_TMP=rootfs_tmp.img

chmod +x bin/* -R
cd bin

if [ -d "$ORG_FS_DIR" ]; then
sudo rm -rf $ORG_FS_DIR
fi

#step 1：解压原始文件系统 

echo "1. ungunziping img file..."

./ungunzip.sh ../$ROOTFS_IMG $ORG_FS_DIR

#step 2: 将需要的文件拷贝至文件系统中

echo "2. copying files..."

# 生成固件版本号，文件名称为firmware_ver
./local_ver_build.sh . firmware_ver

# 版本号文件拷贝到“/”目录下
mv firmware_ver $ORG_FS_DIR

#cp -a ../normal_sys_app/* $NORMAL_SYS_APP_DIR

# 重新制作镜像，23MB - 23552
echo "3. gziping img file..."
./my_mkrootfs.sh $ORG_FS_DIR $OUTPUT_IMAGE 23552

mv $OUTPUT_IMGE_GZ $OUTPUT_IMAGE

./mkimage -A arm -O linux -T ramdisk -C none -a 0x0 -e 0x0 -n 'rootfs' -d $OUTPUT_IMAGE $RFS_TMP

mv $RFS_TMP ../$OUTPUT_IMAGE

#step 4:删除临时文件
echo "4. removing tmp files..."
#umount $ORG_FS_DIR
sudo rm -rf $ORG_FS_DIR
sudo rm -rf $OUTPUT_IMAGE

echo ""
echo "!!!! generate img file: " $OUTPUT_IMAGE "!!!!"
echo ""


cd ../
