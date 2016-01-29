#!/bin/sh
#export TOOLCHAIN=/home/andrea/android-ndk-r8e/standalone-toolchains/android-9-arm-4.7
export TOOLCHAIN=/opt/linux_devkit
#export CROSS_COMPILE=arm-linux-androideabi
export CROSS_COMPILE=arm-arago-linux-gnueabi
export PATH=$TOOLCHAIN/bin:$TOOLCHAIN/$CROSS_COMPILE/bin:$PATH
export SYSROOT=$TOOLCHAIN/
export CC=$CROSS_COMPILE-gcc
export CXX=$CROSS_COMPILE-g++
export CPP=$CROSS_COMPILE-cpp
export CFLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=neon -O3"
export LDFLAGS="-Wl,--fix-cortex-a8"
export LIBJPEG_TURBO_BASE_DIR=/mnt/hgfs/src/signalway/SW_LPR_Main/BasicModule/src/public/SWJPEGFilter/libjpeg-turbo-1.3.0

echo "Configuring..."

#sh $LIBJPEG_TURBO_BASE_DIR/configure --host=arm-linux-androideabi --prefix=$SYSROOT/usr
sh $LIBJPEG_TURBO_BASE_DIR/configure --host=arm-arago-linux-gnueabi
