#!/bin/sh
echo "****************************************"
echo "make venus app upgrade image!!!"
echo "****************************************"

if [ "$1"x = x ];then
	IMAGE_PKG=venus_app.img
else
	IMAGE_PKG=$1.img
fi
UPGRADE_IMAGE=venus_app.img
IMAGE_DIR=/tmp/tmp/
IMAGE_TYPE=28

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
cp -vf ./ubifs_app.img ${IMAGE_DIR}

cd ${IMAGE_DIR}
./HvUpdatePackCreater $IMAGE_TYPE    #mac

cd -
#mv -v  ${IMAGE_DIR}${UPGRADE_IMAGE} ./
echo "****************************************"
echo "make venus app upgrade image sucessful!"
echo "****************************************"

echo "****************************************"
echo "copy app upgrade image to root path! "
echo "****************************************"
#cp -vf ${UPGRADE_IMAGE} ../../../
cp -vf ${IMAGE_DIR}${UPGRADE_IMAGE} ../../../$IMAGE_PKG

rm -rf ${IMAGE_DIR}
