#!/bin/sh
#
# 功能：制定UBI格式镜像
# 使用：请用root权限执行
# 编写者：Late Lee
# 注1：如果不懂使用，请联系脚本作者。
# 注2：本脚本仅在ubuntu系统中测试通过。

if [ $# != 4 ]; then
    echo "usage:" $0 "[dir] [img file] [flash partition size(in MiB)] [vol name]"
    echo "eg: 从rootfs目录制作文件系统镜像rootfs.img到70MB的flash分区，卷名为rootfs，命令如下："
    echo $0 "rootfs rootfs.img 70 rootfs"
    exit
fi

ROOTFS=$1
IMGFILE=$2
SECTION_SIZE=$3
VOL_NAME=$4
TMPFILE=ubifs.img

###################################################
# 根据http://processors.wiki.ti.com/index.php/UBIFS_Support计算
# PEB大小
SP=128
# LEB大小
SL=$[ SP - 2*2 ]
# 计算PEB块数
P=$[ SECTION_SIZE*1024/128 ]
# 保留PEB数
B=$[ P/100 ]
# 
O=$[ SP - SL ]

UBI_OVERHEAD=$[ ((B + 4) * SP + O * (P - B - 4)) / 128 ]

VOL_SIZE=$[ (P - UBI_OVERHEAD) * SP / 1024 + 1 ]

C_VALUE=$[ (P - UBI_OVERHEAD) * SP / SL ]

echo "debug: calc UBI info"
echo "vol size:" $VOL_SIZE
echo "vol name:" $VOL_NAME
echo "c value:" $C_VALUE

################################################################

echo "debug: generating cfg file"
### 生成cfg文件
CFG_FILE=ubinize_auto.cfg
echo "[ubifs]" > $CFG_FILE
echo "mode=ubi" >> $CFG_FILE
echo "image="$TMPFILE >> $CFG_FILE
echo "vol_id=0" >> $CFG_FILE
echo "vol_size="${VOL_SIZE}MiB >> $CFG_FILE
echo "vol_type=dynamic" >> $CFG_FILE
echo "vol_name="${VOL_NAME} >> $CFG_FILE
echo "vol_flags=autoresize" >> $CFG_FILE

################################################################

chmod +x mkfs.ubifs
chmod +x ubinize

echo "debug: generating ubi file"
./mkfs.ubifs -r $ROOTFS -m 2048 -e 126976 -c $C_VALUE -o $TMPFILE
./ubinize -o $IMGFILE -m 2048 -p 128KiB -s 512 -O 2048 $CFG_FILE

sudo rm -rf $TMPFILE

#sudo rm -rf $CFG_FILE

echo "Done!"
