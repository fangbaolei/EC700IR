#!/bin/sh
echo "make dir.............."

mkdir -p /tmp/SIP/temp 
mkdir -p /tmp/SIP/extemp

chmod 777 /tmp/SIP/
chmod 777 /tmp/SIP/temp
chmod 777 /tmp/SIP/extemp

rm -rf /tmp/SIP/temp/* /tmp/SIP/extemp/* 

echo "complie libosip......."

cd ./libosip2-3.6.0
make clean
./configure --host=arm-arago-linux-gnueabi --prefix=/tmp/SIP/temp/ --enable-static

make

make install

echo "compile libeXosip....."


cd ../libeXosip2-3.6.0

make clean
./configure --host=arm-arago-linux-gnueabi --prefix=/tmp/SIP/extemp/ --enable-static PKG_CONFIG_PATH=/tmp/SIP/temp/lib/pkgconfig/ --disable-openssl
make 
make install

