#!/bin/sh

## file: /etc/reloadko.sh
## auth: panxw
## disc:
## 重新加载驱动
## 适用于所有平台，包括一体机200W、500W的主从CPU和单板
## 
## 2012-02-15 lijj：
## 由于将loadko.sh放到/usr/local/signalway目录，此文件作相应修改
##
## 2012-02-23
## 为兼容loader，将/etc/reloadko.sh链接到
## /usr/local/signalway/reloadko.sh

# 最大测试次数
max_test_count=30

# 卸载驱动
# 参数：驱动名称
rmmod_device() {
	device_name=$1
	
	if [ -z $device_name ]; then
		# 名字为空返回失败
		return 1;
	fi
	
	i=$max_test_count
	while [ $i -ne 0 ]; do
		tmp=`lsmod | awk "\\$1==\"$device_name\" {print \\$3}"`
		if [ -z "$tmp" ]; then
			# 不存在
			return 0
		else
			## 存在驱动且资源无占用
			if [ $tmp -eq 0 ]; then
				rmmod ${device_name}
				return 0
			fi
		fi
		
		i=`expr $i - 1`
	done
	
	return 1;
}

# 加载驱动
# 参数1：驱动文件名
# 参数2：驱动名称
insmod_device() {
	if [ $# -lt 2 ] || [ -z $1 ] || [ -z $2 ]; then
		# 名字和文件为空返回失败
		return 1;
	fi
	
	device_file=$1
	device_name=$2
	
	insmod $device_file
	
	# 检查是否加载成功
	i=$max_test_count
	while [ $i -ne 0 ]; do
		tmp=`lsmod | grep $device_name`
		if [ -n "$tmp" ]; then
			## 存在驱动
			return 0;
		fi
		i=`expr $i - 1`
	done
	
	return 1;
}

## 卸载驱动
unload_devices() {
	rmmod_device "slw_pci"
	if [ $? -ne 0 ]; then
		echo "rmmod device 'slw_pci' error!"
		return 1
	fi
	
	rmmod_device "DSPLink"
	if [ $? -ne 0 ]; then
		echo "rmmod device 'DSPLink' error!"
		return 1
	fi
	
	rmmod_device "cmemk"
	if [ $? -ne 0 ]; then
		echo "rmmod device 'cmemk' error!"
		return 1
	fi
	
	if [ -c /dev/swpci ]; then
		rm -f /dev/swpci
	fi
	rm -f /dev/DSPLink
	
	return 0
}

## 加载驱动
load_devices() {
	insmod_device "/lib/modules/DSPLink.ko" "DSPLink"
	if [ $? -ne 0 ]; then
		echo "insmod device 'DSPLink' error!"
		return 1
	fi
	
	mknod /dev/DSPLink c `awk "\\$2==\"DSPLink\" {print \\$1}" /proc/devices` 0
	
	if [ -f /usr/local/signalway/loadko.sh ]; then
		/bin/sh /usr/local/signalway/loadko.sh
	fi
	
	return 0
}

# 统一将core文件放到/corefile目录下
echo "/log/core-%e-%p-%t" > /proc/sys/kernel/core_pattern

unload_devices
if [ $? -ne 0 ]; then
	echo "unload devices error!"
	exit 1
fi

load_devices
if [ $? -ne 0 ]; then
	echo "load devices error!"
	exit 1
fi
