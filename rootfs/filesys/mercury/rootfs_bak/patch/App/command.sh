#!/bin/sh
#命令管理脚本，主要包含了一系列的函数，通过脚本调用具体的函数，调用方法如下
#command.sh 函数名 参数
#成功返回0，失败返回对应的错误代码

#功能: 硬盘是否存在
#返回值：硬盘个数
disk_is_exist()
{
	fdisk -l|wc -l
}

#功能:硬盘分区
#输入参数：每个分区大小
fdisk_disk()
{
	#判断是否有硬盘
	if [ `disk_is_exist` -eq 0 ];then
		echo "1"
		return
	fi

	#先删除现有的分区
	hasDiskNum=`fdisk -l|egrep "/dev/sda[1-9]"|grep -v grep|wc -l`
	while [ $hasDiskNum -gt 0 ]
	do
		echo "=========remove disk partition = $hasDiskNum============"
		if [ $hasDiskNum -gt 1 ]
		then
			fdisk /dev/sda <<-!
				d
				$hasDiskNum
				w
			!
		else
			fdisk /dev/sda <<-!
				d
				w
			!
		fi		
		hasDiskNum=`fdisk -l|egrep "/dev/sda[1-9]"|grep -v grep|wc -l`
	done
	
	#硬盘分区个数
	num=`expr $# - 1`
	if [ $num -gt 4 ]
	then
		num=`expr $num + 1`
	fi
	
	i=1
	while [ $i -le $num ]
	do
	  shift
		size=+$1"K"
		
		echo "===========create disk partition $i,size =$size============="
		#最后一个分区
		if [ $i -eq $num ]
		then
			if [ $i -le 4 ]
			then
				fdisk /dev/sda <<-!
					n
					p
					$i
					
					
					w
				!
			else
				fdisk /dev/sda <<-!
					n
			    	
					
					w
				!
			fi
		#小于4个分区的分区方式
		elif [ $i -lt 4 ]
		then
			fdisk /dev/sda <<-!
				n
				p
				$i
				
				$size
				w
			!
		#第4个分区的分区方式
		elif [ $i -eq 4 ]
		then
			fdisk /dev/sda <<-!
				n
				e
				
				
				w
			!
		#大于4个分区的分区方式
		else
			fdisk /dev/sda <<-!
				n
				
				$size
				w
			!
		fi
		i=`expr $i + 1`
	done
#	partprobe	
	echo "0"
}

#功能    : 格式化所有硬盘
#输入参数：无
format_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi

	fdisk -l|grep Linux|awk '{print $1 " " $4}'|while read DISK INODE
	do
		INODE=`echo $INODE|awk -F '+' '{print $1}'`
		INODE=`expr $INODE / 256 + 50000`
		echo "==================mkfs.ext3 -N $INODE -T largefile $DISK================="
		mkfs.ext3 -N $INODE -T largefile $DISK <<-!
		
		!
		if [ $? -ne 0 ];then
			return $?
		fi
		sleep 10
	done
	return 0
}

#功能    :挂接硬盘
#输入参数:分区 目录 参数
mount_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi
	
	if [ ! -d $2 ]
	then
		mkdir -p $2
	fi
	mount $1 $2 $3
	echo $?
}
#功能    :卸载硬盘
#输入参数:分区
umount_disk()
{
	umount -f $1
	echo $?
}

#功能:检测并修复硬盘
#输入参数:0表明简单的检测，1表明坏道检测
check_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi
	
	if [ $1 -eq 0 ]
	then
		fdisk -l|grep Linux|awk '{print $1}'|while read DISK
		do
			e2fsck -p $DISK
			e2fsck -y $DISK
		done		
	else
		echo "==========badblocks -s -v /dev/sda============="
		which badblocks
		if [ $? -eq 0 ];then
			badblocks -s -v /dev/sda
			if [ $? -ne 0 ];then
				echo $? > $logFile
			fi
		else
			echo "===============can not find badblocks==========="
		fi		
	fi
	echo $?
}

#LprApp进程初始化脚本
init_LprApp()
{
	#卸载所有驱动
	lsmod|grep -v Module|while read LINE
	do
		rmmod $LINE
	done
	
	#装载DSPLink
	insmod /lib/modules/DSPLink.ko
	rm -f /dev/DSPLink
	mknod /dev/DSPLink c `cat /proc/devices|grep DSPLink|awk '{print $1}'` 0
	echo $?
}

#功能:执行一条外部命令
#输入: 命令 参数 返回值文件
exec_cmd()
{
	$1
}
     
#条件分支  
command=$1
shift
param=$@
case $command in
  "fdisk_disk")  return `fdisk_disk $param`
	;;
	
	"format_disk") return `format_disk $param`
	;;
	
	"check_disk")  return `check_disk $param`
	;;
	
	"mount_disk")  return `mount_disk $param`
	;;
	
	"umount_disk") return `umount_disk $param`
	;;
	
	"init_LprApp") return `init_LprApp`
	;;
	
	*)
	(exec_cmd "$command $param"; return $?)
	;;
esac
