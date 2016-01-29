#!/bin/sh

DSPAPP=bin/ipnc_rdk_fw_c6xdsp.xe674_e
ARMAPP=LprApp_e

if [ ! -x DecryptApp ]
then
    chmod u+x DecryptApp
fi

echo "Decrypt app file................"

./DecryptApp -f $DSPAPP -t 1 -e 0 -d /tmp/

./DecryptApp -f $ARMAPP -t 0 -e 0 -d /tmp/
ret=$?
if [ $ret -ne 0 ]; then
    echo "Decrypt arm app failed, will not going on!!!"
    exit
fi

chmod u+x /tmp/*

#read only filesystem
#echo "create soft link................"
#ln -sf /tmp/ipnc_rdk_fw_c6xdsp.xe674 bin/ipnc_rdk_fw_c6xdsp.xe674
#ln -sf /tmp/LprApp  ./LprApp


