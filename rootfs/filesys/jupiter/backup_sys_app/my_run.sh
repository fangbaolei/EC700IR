#!/bin/sh
echo "in run.sh"

# 挂载(在S99finish.sh中挂载，此处不再挂载)
#echo "mounting our ubifs...."
#sh /etc/rcS.d/mount_ubifs.sh

sync
sleep 1

# 设置IP
setip_fromeeprom

# 灭灯(内核将两个led都点亮，此处灭掉红色led)
echo 0 > /sys/class/leds/led_red/brightness

# 将omap系列的tty链接成传统的tty设备名称
ln -s /dev/ttyO1 /dev/ttyS1
ln -s /dev/ttyO3 /dev/ttyS3
ln -s /dev/ttyO4 /dev/ttyS4

# 把采集图像程序链接到home目录，否则程序运行不正常
ln -sf /opt/ipnc/ipc_arch  /home/root/ipc_arch

#加载FPGA
# 木星没有FPGA了
#cd /opt/ipnc
#./load_fpga fpga.xsvf

#启动图像采集测试程序，后台运行
#cd /home/root/ipc_arch/bin
#./ipc_client.out &


# 运行守护进程
cd /opt/ipnc
./deamon &
