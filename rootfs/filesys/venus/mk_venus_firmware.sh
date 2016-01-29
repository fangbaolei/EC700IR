#!/bin/sh
echo "*******************************************"
echo "make venus firmware upgrade image!!!"
echo "*******************************************"
IMAGE_DIR=tmp/
UPGRADE_IMAGE=venus_firmware.img
IMAGE_TYPE=26

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
cp -vf ./venus_rootfs.img ${IMAGE_DIR}rootfs.img
cp -vf ./0_uboot_kernel/uImage.venus ${IMAGE_DIR}uImage.venus

cd ${IMAGE_DIR}
./HvUpdatePackCreater $IMAGE_TYPE    #mac

cd -
mv -v  ${IMAGE_DIR}${UPGRADE_IMAGE} ./
echo "*******************************************"
echo "make venus firmare upgrade image sucessful!"
echo "*******************************************"
rm -rf ${IMAGE_DIR}
