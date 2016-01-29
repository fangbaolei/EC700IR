#!/bin/sh
echo "update inc and lib"
EXPORT_LIB_PATH=../../Application/GB28181App/lib/
EXPORT_INC_PATH=../../Application/GB28181App/include
cp -vf /tmp/SIP/temp/lib/libosip2.a ${EXPORT_LIB_PATH}
cp -vf /tmp/SIP/temp/lib/libosipparser2.a ${EXPORT_LIB_PATH}
cp -vf /tmp/SIP/extemp/lib/libeXosip2.a ${EXPORT_LIB_PATH}

cp -vrf /tmp/SIP/temp/include/osip2 ${EXPORT_INC_PATH}
cp -vrf /tmp/SIP/temp/include/osipparser2 ${EXPORT_INC_PATH}
cp -vrf /tmp/SIP/extemp/include/eXosip2 ${EXPORT_INC_PATH}

