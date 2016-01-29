#!/bin/sh

echo "****************************************"
echo "make jupiter uboot upgrade image!!!"
echo "****************************************"
IMAGE_DIR=tmp/
UPGRADE_IMAGE=venus_uboot.img
UPGRADE_IMAGE_NEW=jupiter_uboot.img
IMAGE_TYPE=27

# 部分文件与金星平台共用，用此宏定义
INPUT_DIR=../venus
OUTPUT_DIR=$PWD

cd $INPUT_DIR

echo "check tmp dir"
if [ -d "$IMAGE_DIR" ]; then
    echo "tmp dir is exit"
else
    mkdir -v ${IMAGE_DIR}
fi

echo "check bin mode"
if [ ! -x HvUpdatePackCreater ]
then
    echo "chmod packcreater bin"
    chmod u+x HvUpdatePackCreater
fi

echo "copy PackCreater bin"
cp -vf ./HvUpdatePackCreater ${IMAGE_DIR}

echo "copy image file !"
echo "....................."
cp -vf ./0_uboot_kernel/u-boot.bin ${IMAGE_DIR}
cp -vf ./0_uboot_kernel/u-boot.min.bin ${IMAGE_DIR}

cd ${IMAGE_DIR}
./HvUpdatePackCreater $IMAGE_TYPE    #mac

cd -
mv -v  ${IMAGE_DIR}${UPGRADE_IMAGE} $OUTPUT_DIR/$UPGRADE_IMAGE_NEW
echo "****************************************"
echo "make jupiter uboot upgrade image sucessful!"
echo "****************************************"
rm -rf ${IMAGE_DIR}
