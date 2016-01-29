#!/bin/bash

#
# 功能: 制作备份根文件系统镜像。
# 
# 请使用root权限执行本脚本！

OUTPUT_IMAGE=venus_rootfs_back.img
OUTPUT_IMGE_GZ=$OUTPUT_IMAGE.gz
ORG_FS_DIR=rootfs_org

MOUNT_DIR=$ORG_FS_DIR/etc/rcS.d

# 原始根文件系统镜像
ROOTFS_IMG=rootfs.dm8127.img.gz
#临时专用
RFS_TMP=rootfs_tmp.img.gz
RFS_1=rootfs_tmp.img

MOUNT_FILES="backup_sys_app/script/mount_ubifs.sh backup_sys_app/script/umount_ubifs.sh"
MOUNT_FILES1="../backup_sys_app/script/mount_ubifs.sh ../backup_sys_app/script/umount_ubifs.sh"

#判断必要的文件是否存在。若不存在则制作失败。
for i in $MOUNT_FILES; do
    if [ ! -f $i ]; 
	then
        echo -e "Error!! file not found:" $i
        exit
    fi
done

chmod +x bin/*.sh
cd bin

if [ -d "$ORG_FS_DIR" ]; then
sudo rm -rf $ORG_FS_DIR
fi

#step 1：解压原始文件系统 

echo "1. ungunziping img file..."

./ungunzip.sh ../$ROOTFS_IMG $ORG_FS_DIR

#step 2: 将需要的文件拷贝至文件系统中
echo "2. copying files..."

# 所有文件都添加可执行属性
chmod +x ../backup_sys_app/* -R

# 生成固件版本号，文件名称为firmware_ver
./local_ver_build.sh . firmware_ver

# 版本号文件拷贝到“/”目录下
mv firmware_ver $ORG_FS_DIR

# 挂载脚本(与正式系统不同，故要修改)
for i in $MOUNT_FILES1; do
    sudo cp -fp $i $MOUNT_DIR/`basename $i`;
done

# 重新制作镜像
echo "3. gziping img file..."
./my_mkrootfs.sh $ORG_FS_DIR $OUTPUT_IMAGE 23552

mv $OUTPUT_IMGE_GZ $OUTPUT_IMAGE

./mkimage -A arm -O linux -T ramdisk -C none -a 0x0 -e 0x0 -n 'rootfs_bak' -d $OUTPUT_IMAGE $RFS_TMP

mv $RFS_TMP ../$OUTPUT_IMAGE

#umount $ORG_FS_DIR
sudo rm -rf $ORG_FS_DIR
sudo rm -rf $RFS_1
sudo rm -rf $OUTPUT_IMAGE

echo ""
echo "!!!! generate img file: " $OUTPUT_IMAGE "!!!!"
echo ""

cd ../
