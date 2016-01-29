#!/bin/sh
echo "****************************************"
echo "make venus uboot upgrade image!!!"
echo "****************************************"
IMAGE_DIR=tmp/
UPGRADE_IMAGE=venus_uboot.img
IMAGE_TYPE=27

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
mv -v  ${IMAGE_DIR}${UPGRADE_IMAGE} ./
echo "****************************************"
echo "make venus uboot upgrade image sucessful!"
echo "****************************************"
rm -rf ${IMAGE_DIR}
