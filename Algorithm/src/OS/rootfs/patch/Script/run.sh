#!/bin/sh
## 文件名称：run.sh
## 功能：运行应用程序，本文件用于一体机主从端、单板的程序启动

## 调试版本(需要将ARM、DSP端程序放到/usr/local/signalway目录)：
## 主端：
## 1、loader默认会自动读取同一目录的fpga.xsvf文件
## 2、CamApp默认会自动读取同一目录的CamDsp.out文件
## 从端(单板)：
## LprApp默认会自动读取同一目录的HvDsp.out
##

## 正式版本：
## 由loader从Flash读取程序

USERPROG=loader
USERARGS=/dev/swdev

cd /usr/local/signalway

# 一体机主
if [ -x CamApp ]; then
    ## web server
    if [ -d /usr/sbin/goaheadbin/bin/ ]; then
        /usr/sbin/goaheadbin/bin/webs &
    fi

    ## arp ping
    if [ -x /usr/sbin/ifplugd ]; then
        /usr/sbin/ifplugd -a -d 0 -r /etc/ifplugd.action
    fi
    ./loader
    ./CamApp

# 一体机从，单板
elif [ -x LprApp ]; then
    ./LprApp

# 正式版
elif [ ! -x CamApp ] && [ ! -x LprApp ] && [ -x loader ]; then
    ## web server
    if [ -d /usr/sbin/goaheadbin/bin/ ]; then
        /usr/sbin/goaheadbin/bin/webs &
    fi

    ## arp ping
    if [ -x /usr/sbin/ifplugd ]; then
        /usr/sbin/ifplugd -a -d 0 -r /etc/ifplugd.action
    fi
    echo -n "Starting $USERPROG ... "
    ##Run user program
    ./$USERPROG $USERARGS
else
    echo "no program found."
fi
