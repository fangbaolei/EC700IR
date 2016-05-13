#!/bin/bash
#########################################################
# 本脚本用于拷贝编译生成的程序更新到做包目录
# 方便拷贝到同时，防止遗漏
#########################################################

#if err exit shell
set -e

usage_func() {
	printf "Usage: $0 [OPTION]\n"
	printf "OPTIONS:\n"
	printf "\t1-${PROJECT_EN[1]}(${PROJECT_CN[1]}-185前端)\n"
	printf "\t2-${PROJECT_EN[2]}(${PROJECT_CN[2]}-185前端)\n"
	printf "\t3-${PROJECT_EN[3]}(${PROJECT_CN[3]}-178前端)\n"
	printf "\t4-${PROJECT_EN[4]}(${PROJECT_CN[4]}-185前端)\n"
	printf "\t5-${PROJECT_EN[5]}(${PROJECT_CN[5]}-无前端)\n"
	printf "\t6-${PROJECT_EN[6]}(${PROJECT_CN[6]}-249前端)\n"
	printf "\t7-${PROJECT_EN[7]}(${PROJECT_CN[7]}-816前端)\n"
}

VENUS_TOLL_GATE=1		#收费站
VENUS_TRAFFIC_GATE=2	#卡口
VENUS_EPOLICE=3			#电警
VENUS_CAMERA=4			#纯相机
JUPITER_DOMECAMERA=5	#球机
VENUS_TRAFFIC_GATE_BU=6 #249卡口
VENUS_TRAFFIC_GATE_700W=7 #816卡口

PROJECT_EN=([0]="NULL"
						[1]="TollGate"
						[2]="TrafficGate"
						[3]="EPolice"
						[4]="Camera"
						[5]="DomeCamera"
                        [6]="TrafficGateBU"
                        [7]="TrafficGate700W")
PROJECT_CN=([0]="NULL" 
						[1]="视频流收费站"
						[2]="卡口"
						[3]="电子警察"
						[4]="纯相机"
						[5]="违停抓拍"
                        [6]="卡口"
                        [7]="海外卡口")


#if [ $# -ge 1 ];then
#	PROJECT=$1
#else
#	usage_func 
#	printf "Please input your choose:"
#	read PROJECT
#fi
#跳过选择分型，直接选择700W卡口
PROJECT=7

printf "your choose:$PROJECT-"
if [ $PROJECT -lt 1 ] || [ $PROJECT -gt 7 ];then
#echo "INVALID OPTION!"
	printf "\e[0;31;1mINVALID OPTION!\e[0m\n"
	exit 1
fi
printf "${PROJECT_EN[${PROJECT}]}(${PROJECT_CN[${PROJECT}]})\n"	

#####################################################################
MAJOR_VERSION=3.1.107
MINOR_VERSION=1000	#default


if [ $PROJECT -eq $VENUS_EPOLICE ]
then
    HARDWARE_NAME="PCC600一体机"
    ROOTFS_DIR=rootfs/filesys/venus
    MK_APP_SHELL=mk_venus_app.sh
elif [ $PROJECT -eq $JUPITER_DOMECAMERA ]
then
    HARDWARE_NAME="SDC200一体机"
    ROOTFS_DIR=rootfs/filesys/jupiter
    MK_APP_SHELL=mk_jupiter_app.sh
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_BU ]
then
    HARDWARE_NAME="PCC200B一体机"
    ROOTFS_DIR=rootfs/filesys/venus
    MK_APP_SHELL=mk_venus_app.sh
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_700W ]
then
    HARDWARE_NAME="EC700红外一体机"
    ROOTFS_DIR=rootfs/filesys/venus
    MK_APP_SHELL=mk_venus_app.sh
else
    HARDWARE_NAME="PCC200一体机"
    ROOTFS_DIR=rootfs/filesys/venus
    MK_APP_SHELL=mk_venus_app.sh
fi

XML_CONFIG_PATH=Config/
START_TIME=$(date +%s)
CONFIG_H=config.h
VERSION_H=DspBuildNo.h

if [ -e $VERSION_H ];then
	MINOR_VERSION=`cat $VERSION_H | 
					grep "PSZ_DSP_BUILD_NO" | 
					awk '{print $5}' | 
					tr -d ';""\n\r'`
fi

#if [ "$MINOR_VERSION"x = x ];then
#	MINOR_VERSION=1000
#fi

#APP_VERSION=${MAJOR_VERSION}.${MINOR_VERSION}
APP_VERSION=${MINOR_VERSION}

IMAGE_NAME="App_${HARDWARE_NAME}_${APP_VERSION}_${PROJECT_CN[${PROJECT}]}"

#不同的方案需要修改拷贝不同的配置
if [ $PROJECT -eq $VENUS_TOLL_GATE ];then
	CONFIG_FILE=${XML_CONFIG_PATH}LprApp_TollGate_venus.xml
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE ];then
	CONFIG_FILE=${XML_CONFIG_PATH}LprApp_TrafficGate_venus.xml
elif [ $PROJECT -eq $VENUS_EPOLICE ];then
	CONFIG_FILE=${XML_CONFIG_PATH}LprApp_EPolice_venus.xml
elif [ $PROJECT -eq $JUPITER_DOMECAMERA ];then
	CONFIG_FILE=${XML_CONFIG_PATH}LprApp_DomeCamera_Jupiter.xml
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_BU ];then
    CONFIG_FILE=${XML_CONFIG_PATH}LprApp_TrafficGate_venus.xml #the same as PCC200
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_700W ];then
    CONFIG_FILE=${XML_CONFIG_PATH}LprApp_TrafficGate_venus_ec700.xml 
else 
	CONFIG_FILE=${XML_CONFIG_PATH}LprApp_Camera_venus.xml
fi

if [ $PROJECT -eq $VENUS_EPOLICE ]
then
    FPGA_FILE=$ROOTFS_DIR/1_fpga/fpga_178.xsvf
    FPGA_VER_FILE=$ROOTFS_DIR/1_fpga/fpga_178_ver.dat
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_BU ]
then
    FPGA_FILE=$ROOTFS_DIR/1_fpga/fpga_249.xsvf
    FPGA_VER_FILE=$ROOTFS_DIR/1_fpga/fpga_249_ver.dat
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_700W ]
then
    FPGA_FILE=$ROOTFS_DIR/1_fpga/fpga_816.xsvf
    FPGA_VER_FILE=$ROOTFS_DIR/1_fpga/fpga_816_ver.dat
else
    FPGA_FILE=$ROOTFS_DIR/1_fpga/fpga_185.xsvf
    FPGA_VER_FILE=$ROOTFS_DIR/1_fpga/fpga_185_ver.dat
fi


if [ $PROJECT -eq $JUPITER_DOMECAMERA ]
then
    DEAMON_FILE=${XML_CONFIG_PATH}deamon_jupiter.xml
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_BU ]
then
    DEAMON_FILE=${XML_CONFIG_PATH}deamon_2a.xml
elif [ $PROJECT -eq $VENUS_TRAFFIC_GATE_700W ]
then
    DEAMON_FILE=${XML_CONFIG_PATH}deamon_2a.xml

else
    DEAMON_FILE=${XML_CONFIG_PATH}deamon.xml
fi

SHELL_FILE=${XML_CONFIG_PATH}command.sh

COMPILE_BIN_PATH=bin/ARM/
SVN_BIN_PATH=swpa/src/dm8127/ipc/
SVN_DRIVER_PATH=${SVN_BIN_PATH}kermod/
SVN_ARM_APP_PATH=${SVN_BIN_PATH}bin/release/
SVN_DSP_APP_PATH=${SVN_BIN_PATH}firmware/

APP_PATH=$ROOTFS_DIR/App/
DRIVER_PATH=${APP_PATH}ko/
MCFW_PATH=${APP_PATH}bin/

echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy config to make image path"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
cp -vf ${CONFIG_FILE} ${APP_PATH}LprApp.xml
cp -vf ${DEAMON_FILE} ${APP_PATH}deamon.xml

echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy arm bin to make image path!!!!"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
cp -vf ${COMPILE_BIN_PATH}LprApp ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}OnvifApp ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}RtspApp ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}UpgradeApp ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}command ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}deamon ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}log ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}GB28181App ${APP_PATH}
cp -vf ${COMPILE_BIN_PATH}2AApp ${APP_PATH}

echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy driver ko to make image path!!!"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
cp -vf ${SVN_DRIVER_PATH}osa_kermod.ko ${DRIVER_PATH}
cp -vf ${SVN_DRIVER_PATH}syslink.ko ${DRIVER_PATH}
cp -vf ${SVN_DRIVER_PATH}ti81xxfb.ko ${DRIVER_PATH}
cp -vf ${SVN_DRIVER_PATH}ti81xxhdmi.ko ${DRIVER_PATH}
cp -vf ${SVN_DRIVER_PATH}vpss.ko ${DRIVER_PATH}

echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy mcfw arm bin to make image path!!!!"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
cp -vf ${SVN_ARM_APP_PATH}fw_load.out ${MCFW_PATH}
cp -vf ${SVN_ARM_APP_PATH}mem_rdwr.out ${MCFW_PATH}
cp -vf ${SVN_ARM_APP_PATH}remote_debug_client.out ${MCFW_PATH}
cp -vf ${SVN_ARM_APP_PATH}sys_pri.out ${MCFW_PATH}
cp -vf ${SVN_BIN_PATH}bin/linux_prcm_ipcam ${MCFW_PATH}

echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy mcfw dsp and m3 bin to make image path"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'

if [ ${CONFIG_FILE} == ${XML_CONFIG_PATH}LprApp_EPolice_venus.xml ];then
    cp -vf ${SVN_DSP_APP_PATH}EP_dsp/ipnc_rdk_fw_c6xdsp.xe674 ${MCFW_PATH}
elif [ ${CONFIG_FILE} == ${XML_CONFIG_PATH}LprApp_DomeCamera_Jupiter.xml ];then  
 	cp -vf ${SVN_DSP_APP_PATH}PK_dsp/ipnc_rdk_fw_c6xdsp.xe674 ${MCFW_PATH}
elif [ ${CONFIG_FILE} == ${XML_CONFIG_PATH}LprApp_TrafficGate_venus.xml ];then
	cp -vf ${SVN_DSP_APP_PATH}TrafficGate_dsp/ipnc_rdk_fw_c6xdsp.xe674 ${MCFW_PATH}
elif [ ${CONFIG_FILE} == ${XML_CONFIG_PATH}LprApp_TrafficGate_venus_ec700.xml ];then
	cp -vf ${SVN_DSP_APP_PATH}TrafficGate_dsp/ipnc_rdk_fw_c6xdsp.xe674 ${MCFW_PATH} #the same as PCC200B
else
	cp -vf ${SVN_DSP_APP_PATH}ipnc_rdk_fw_c6xdsp.xe674 ${MCFW_PATH}
fi

cp -vf ${SVN_DSP_APP_PATH}ipnc_rdk_fw_m3vpss.xem3 ${MCFW_PATH}
cp -vf ${SVN_DSP_APP_PATH}ipnc_rdk_fw_m3video.xem3 ${MCFW_PATH}


echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "copy shell file to make image path"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
cp -vf ${SHELL_FILE} ${APP_PATH}

if [ $PROJECT -eq $JUPITER_DOMECAMERA ]
then
    rm -f ${APP_PATH}fpga.xsvf     
    rm -f ${APP_PATH}fpga_ver.dat  
    rm -f ${APP_PATH}load_fpga     
else
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    echo "copy fpga file & fpga ver file to make image path"
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    cp -vf ${FPGA_FILE} ${APP_PATH}fpga.xsvf
    cp -vf ${FPGA_VER_FILE} ${APP_PATH}fpga_ver.dat
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "do something you like!!!!"
echo "and now I make app upgrade packet auto!"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
cd $ROOTFS_DIR
sudo ./mkubi_app.sh App
sh $MK_APP_SHELL $IMAGE_NAME


END_TIME=$(date +%s)
INTERVAL_TIME=$(($END_TIME - $START_TIME))
#echo -e '\033[0;33;1myellow\033[0m'
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
echo "make upgrade image success, spend time: ${INTERVAL_TIME} s."
printf "current project: \e[0;32;1m${PROJECT_CN[${PROJECT}]}\e[0m\n"
printf "hardware: \e[0;32;1m$HARDWARE_NAME\e[0m\n"
printf "application version: \e[0;32;1m$APP_VERSION\e[0m\n"
printf "upgrade image: \e[0;32;1m${IMAGE_NAME}.img\e[0m\n"
echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'

