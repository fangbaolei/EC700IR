#!/bin/sh
#export TOOLCHAIN=/home/andrea/android-ndk-r8e/standalone-toolchains/android-9-arm-4.7
export TOOLCHAIN=/opt/linux_devkit
#export CROSS_COMPILE=arm-linux-androideabi
export CROSS_COMPILE=arm-arago-linux-gnueabi
export PATH=$TOOLCHAIN/bin:$TOOLCHAIN/$CROSS_COMPILE/bin:$PATH

echo "Making libjpeg-turbo for Android"
