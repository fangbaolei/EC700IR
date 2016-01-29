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

# 把rs485设备链接为ttyS1，以兼容水星代码
ln -s /dev/ttyO1 /dev/ttyS1

# 把采集图像程序链接到home目录，否则程序运行不正常
ln -sf /opt/ipnc/ipc_arch  /home/root/ipc_arch

#加载FPGA
cd /opt/ipnc
#./load_fpga fpga.xsvf
if [ -e /mnt/nand/realimagefpga185 ] ; then
	echo "185 realimagefpga mode!"
	./load_fpga fpga_185.xsvf
elif [ -e /mnt/nand/realimagefpga178 ] ; then
	echo "178 realimagefpga mode!"
	./load_fpga fpga_178.xsvf
elif [ -e /mnt/nand/realimagefpga249 ] ; then
	echo "249 realimagefpga mode!"
	./load_fpga fpga_249.xsvf	
else
	echo "fakeiamgefpga mode!"
	./load_fpga fpga.xsvf
fi

#启动图像采集测试程序，后台运行
#cd /home/root/ipc_arch/bin
#./ipc_client.out &


# 运行守护进程
cd /opt/ipnc
./deamon &
