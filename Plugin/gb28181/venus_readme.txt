
cd libosip2-3.6.0
./configure --host=arm-arago-linux-gnueabi  
make

复制libosip2-3.6.0文件夹下 include/osip include/osipparser2 到libeXosip2-3.6.0/include 目录下
复制libosip2-3.6.0\src\osip2\.libs文件夹下  libosip2.a libosip2.so libosipparser2.a libosipparser2.so 到 libeXosip2-3.6.0/src下

cd libeXosip2-3.6.0
./configure --host=arm-arago-linux-gnueabi  --disable-openssl
make