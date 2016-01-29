#!/bin/sh
echo "*******************************************"
echo "make jupiter firmware upgrade image!!!"
echo "*******************************************"
IMAGE_DIR=tmp/
UPGRADE_IMAGE=venus_firmware.img
UPGRADE_IMAGE_NEW=jupiter_firmware.img
IMAGE_TYPE=26

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
cp -vf $OUTPUT_DIR/jupiter_rootfs.img ${IMAGE_DIR}rootfs.img
cp -vf ./0_uboot_kernel/uImage.venus ${IMAGE_DIR}uImage.venus

cd ${IMAGE_DIR}
./HvUpdatePackCreater $IMAGE_TYPE    #mac

cd -
mv -v  ${IMAGE_DIR}${UPGRADE_IMAGE} $OUTPUT_DIR/$UPGRADE_IMAGE_NEW
echo "*******************************************"
echo "make jupiter firmare upgrade image sucessful!"
echo "*******************************************"
rm -rf ${IMAGE_DIR}
