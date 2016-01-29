#!/bin/sh
echo "in run.sh"

# 1. 挂载
#echo "mounting our ubifs...."
#sh /etc/rcS.d/mount_ubifs.sh

sync
sleep 1

# 2. 设置IP
setip_fromeeprom

# 3. 灭灯(内核将两个led都点亮，此处灭掉红色led)
echo 0 > /sys/class/leds/led_red/brightness

# 4. 将omap系列的tty链接成传统的tty设备名称
ln -s /dev/ttyO1 /dev/ttyS1
ln -s /dev/ttyO3 /dev/ttyS3
ln -s /dev/ttyO4 /dev/ttyS4

# 5. web程序相关
ln -sf /opt/ipnc/web_publish/* /var/www/

#　6. 解密到tmp目录
./DecryptFile.sh

chmod +x /tmp/*

# 7. 把所有的程序都添加可执行属性
#chmod +x * -R

# 8. 运行web服务器
./boa &


# 9. 运行其它进程

# 加载fpga
# !!!!说明，木星没有FPGA了，不用加载
#./load_fpga fpga.xsvf

# load cmemk
insmod /lib/modules/cmemk.ko phys_start=0x88000000 phys_end=0x8e000000, pools=1x100663296

# 守护进程
./deamon 
