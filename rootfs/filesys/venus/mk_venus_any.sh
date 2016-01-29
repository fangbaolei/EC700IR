#!/bin/sh
echo "***************************************"
echo "make venus any upgrade image!!!"
echo "***************************************"
IMAGE_DIR=$1
UPGRADE_IMAGE=venus_any.img
IMAGE_TYPE=25

if [ "$IMAGE_DIR" = "" ] 
then
    echo "usage:$0 imagedir"    
    exit 0
fi

if [ ! -d $IMAGE_DIR ]
then
    echo "dir $1 is not exit!"    
    exit 0
fi

echo "check bin mode"
if [ ! -x HvUpdatePackCreater ]
then
    echo "chmod packcreater bin"
    chmod u+x HvUpdatePackCreater
fi

echo "copy PackCreater bin"
cp -vf ./HvUpdatePackCreater ${IMAGE_DIR}

echo "......................"
cd ${IMAGE_DIR}
./HvUpdatePackCreater $IMAGE_TYPE    #mac

cd -
mv -v  ${IMAGE_DIR}${UPGRADE_IMAGE} ./
echo "***************************************"
echo "make venus any upgrade image sucessful!"
echo "***************************************"
