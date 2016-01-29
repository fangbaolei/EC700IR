#!/bin/sh
#
#功能：
#	硬盘管理脚本
#	主要管理硬盘的分区和检测
#
#运行方式: 
#	dm.sh type number dir
#
#说明:
#	type说明运行的方式
#	1--表明先按照number分区硬盘，然后在将硬盘挂载到dir目录下的0,1,2...子目录
#	2--表明检测硬盘，步骤是卸载硬盘，检测硬盘，挂接硬盘到dir目录下的0,1,2...子目录
#   3--表明格式化硬盘,然后在挂载硬盘
#	number
#	type=1来说,表明要分多少个区,-1表明自动分配，500G分4个区，1T分6个区
#	type=2来说，无意义
#	type=3来说，表明要格式化那个分区
#	dir 目录
#	运行成功返回0，否则返回1，详细的日志信息输出到控制台，可以通过重定向日志输出到文件
#
#作者:黄国超 编写于2011-09-25
#
#	2011-10-25 修改格式化硬盘命令，在挂载硬盘时，发现分区大于等于3的则需要加2，否则加1	
#   2011-11-02 增加日志输出的末尾为[Finish],以告示脚本运行成功，方便上层调用的判断
#	2011-11-09 修改该脚本为守护进程，通过cmd.txt执行命令,包含内部命令和外部命令

#功能:获得硬盘的大小
#返回值:硬盘大小
disk_size()
{
	if [ `disk_is_exist` -eq 0 ];then
		return $1
	fi
	fdisk -l| grep "Disk /dev/sda"|awk '{print $3}'|awk -F . '{print $1}'
	return 0
}

#功能: 硬盘是否存在
#返回值：硬盘个数
disk_is_exist()
{
	fdisk -l|wc -l
}

#功能:硬盘分区，如果硬盘还没有分区，则分区，否则直接格式化
#输入参数：分区数 
fdisk_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi

	num=$1
	if [ $num -gt 8 ]
	then
		return 1
	fi
	
	if [ $num -le 0 ]
	then
		if [ `disk_size` -ge 1000 ]
		then
			num=6
		else
			num=4
		fi
	fi
	
	hasDiskNum=`fdisk -l|egrep "Linux|Extended"|grep -v grep|wc -l`
	echo "start fdisk,hasDiskNum=$hasDiskNum..."
	#先删除现有的分区
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
		hasDiskNum=`expr $hasDiskNum - 1`
	done
	#然后创建分区
	size=`echo $num \`disk_size\`|awk '{print "+"$2/$1*1000-100"M"}'`
	#最后剩余的空间给日志
	num=`expr $num + 1`
	if [ $num -gt 4 ]
	then
		num=`expr $num + 1`
	fi
	i=1
	while [ $i -le $num ]
	do
		echo "===========create disk partition $i============="
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
				if [ $? -ne 0 ];then
					return $?
				fi
			else
				fdisk /dev/sda <<-!
					n
			    	
					
					w
				!
				if [ $? -ne 0 ];then
					return $?
				fi
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
			if [ $? -ne 0 ];then
				return $?
			fi
		#第4个分区的分区方式
		elif [ $i -eq 4 ]
		then
			fdisk /dev/sda <<-!
				n
				e
				
				
				w
			!
			if [ $? -ne 0 ];then
				return $?
			fi
		#大于4个分区的分区方式
		else
			fdisk /dev/sda <<-!
				n
				
				$size
				w
			!
			if [ $? -ne 0 ];then
				return $?
			fi
		fi
		i=`expr $i + 1`
	done
	return 0	
}
#功能    : 格式化所有硬盘
#输入参数：无
format_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi

	fdisk -l|grep Linux|awk '{print $1}'|while read DISK
	do
		echo "==================mkfs.ext3 -N 1000000 -T largefile $DISK================="
		mkfs.ext3 -N 1000000 -T largefile $DISK <<-!
		
		!
		if [ $? -ne 0 ];then
			return $?
		fi
	done
	return 0
}

#功能    :挂接硬盘
#输入参数:挂接的主目录
mount_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi

	if [ ! -d $1 ]
	then
		mkdir $1
	fi
	dir=0
	num=`fdisk -l|grep Linux|wc -l|awk '{print $1-1}'`
	fdisk -l|grep Linux|awk '{print $1}'|while read DISK
	do
		if [ $dir -lt $num ]
		then
			if [ ! -d $1/$dir ]
			then
				mkdir $1/$dir
			fi
			echo "============mount $DISK $1/$dir================"
			if [ `df|grep $DISK|wc -l` -eq 0 ]
			then
				mount $DISK $1/$dir
				if [ $? -ne 0 ];then
					return $?
				fi
			fi
		else
			echo "=============mount $DISK /log================="
			if [ ! -d /log ]
			then
				mkdir /log
			fi
			if [ `df|grep $DISK|wc -l` -eq 0 ]
			then
				mount $DISK /log
				if [ $? -ne 0 ];then
					return $?
				fi
			fi
		fi
		echo "=============tune2fs -i 0 -c0 $DISK ==============="
		tune2fs -i 0 -c0 $DISK 
		dir=`expr $dir + 1`
	done
	return 0
}
#功能    :卸载硬盘
#输入参数:无
umount_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi

	fdisk -l|grep Linux|awk '{print $1}'|while read DISK
	do
		umount $DISK
	done
	return 0
}

#功能:检测并修复硬盘
#输入参数:无
check_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi
	
	logFile=$1
	#先处理log分区
	umount /dev/sda3
	e2fsck -p /dev/sda3
	e2fsck -y /dev/sda3
	mkdir /log
	mount /dev/sda3 /log
	#优先检测当前再用的分区
	resultDisk="NONE"
	if [ -f /log/Result.dat ]
	then
		if [ `cat /log/Result.dat|wc -l` -eq 0 ]
		then
			index=0
		else
			index=`cat /log/Result.dat`
		fi
		resultDisk=`echo $index|awk '{ if($1 < 4) print "/dev/sda"$1+1; else print "/dev/sda"$1+2;}'`
		echo "==========check $resultDisk first.====================="
		umount $resultDisk
		e2fsck -p $resultDisk
		e2fsck -y $resultDisk
		mkdir /Result_disk
		mkdir /Result_disk/$index
		mount $resultDisk /Result_disk/$index
	fi
	videoDisk="NONE"
	if [ -f /log/Video.dat ]
	then
		if [ `cat /log/Video.dat|wc -l` -eq 0 ]
		then
			index=4
		else
			index=`cat /log/Video.dat`
		fi
		videoDisk=`echo $index|awk '{ if($1 < 4) print "/dev/sda"$1+1; else print "/dev/sda"$1+2;}'`
		echo "=========check $videoDisk first.========================"
		umount $videoDisk
		e2fsck -p $videoDisk
		e2fsck -y $videoDisk
		mkdir /Result_disk
		mkdir /Result_disk/$index
		mount $videoDisk /Result_disk/$index
	fi
	if [ $resultDisk != "-1" ] || [  $videoDisk != "NONE" ]
	then
		echo 0 > $1
	fi
	#在检测其他分区
	fdisk -l|grep Linux|awk '{print $1}'|while read DISK
	do
		if [ $DISK != $resultDisk ] && [ $DISK != $videoDisk ]
		then
			echo "===================e2fsck -p $DISK ======================="
			e2fsck -p $DISK 
			if [ $? -ne 0 ];then
				return $?
			fi
			echo "===================e2fsck -y $DISK ======================="
			e2fsck -y $DISK 
			if [ $? -ne 0 ];then
				return $?
			fi
		fi
	done
	return 0		
}

#功能:检测并修复硬盘 (生产使用)
#输入参数:无
check_disk_2()
{
    echo "=================== check_disk_2 ======================="
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi
	
	fdisk -l|grep Linux|awk '{print $1}'|while read DISK
	do
		echo "===================e2fsck -p $DISK ======================="
		e2fsck -p $DISK 
		if [ $? -ne 0 ];then
			return $?
		fi
		echo "===================e2fsck -y $DISK ======================="
		e2fsck -y $DISK 
		if [ $? -ne 0 ];then
			return $?
		fi
	done
	return 0		
}

detect_disk()
{
	echo "===================check $1==========================="
	df|grep $1|awk '{printf("%s %s\n", $1, $NF)}'|while read DISK DIR
	do
		touch $DIR/test.txt
		if [ $? -ne 0 ]
		then
			umount $DISK
			e2fsck -p $DISK
			e2fsck -y $DISK
			umount $DISK
			mount $DISK $DIR
		fi
		rm $DIR/test	
	done
}

# 加载usb-storage驱动
load_usbstorage()
{
	logFile=$2
	
	if [ ! -z $logFile ] && [ -r $logFile ]
	then
		rm $logFile
	fi
	
	tmp=`lsmod | grep usb_storage`
	if [ -n "$tmp" ]; then
		## 存在驱动，不处理
		echo 'loaded usb-storage'
		echo 0 > $logFile
	else
		# 不存在,加载
		echo 'loading usb-storage'
		insmod /lib/modules/musb_hdrc.ko
		insmod /lib/modules/usb-storage.ko
		echo $? > $logFile
		sleep 10
	fi
}

#硬盘管理脚本
#type   硬盘管理的命令类型
#number 分区个数，目前自动分配，填0
#dir    目录
dm()
{
	type=$1
	number=$2
	dir=$3
	logFile=$4
	
	if [ ! -z $logFile ] && [ -r $logFile ]
	then
		rm $logFile
	fi
	if [ `disk_is_exist` -eq 0 ];then
		echo "not found disk."
		echo 1 > $logFile
	return
	fi
	case $type in
		1)
			umount_disk
 			fdisk_disk $number
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			format_disk
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			mount_disk $dir
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			echo 0 > $logFile
		;;
		2)
			umount_disk
			check_disk $logFile
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			mount_disk $dir
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			echo 0 > $logFile
		;;
		3)
			if [ $number -ge 3 ]
			then
				DISK=/dev/sda`expr $number + 2`
			else
				DISK=/dev/sda`expr $number + 1`
			fi
			umount $DISK
			mkfs.ext3 -N 1000000 -T largefile $DISK <<-!
			
			!
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			mount $DISK $dir/$number 
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			echo 0 > $logFile
		;;
		4)
			detect_disk $dir
			echo 0 > $logFile
		;;
		5)
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
			echo 0 > $logFile
		;;
		6)
			umount_disk
			echo 0 > $logFile
		;;
		7)
			umount_disk
			check_disk_2 $logFile
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			mount_disk $dir
			if [ $? -ne 0 ];then
				echo $? > $logFile
				return
			fi
			echo 0 > $logFile
		;;
		*)
			echo "Usage:$0 type number dir"
			echo "Exam: $0 1 4 Result_Disk"
			echo 1 > $logFile
		;;
	esac
}

#定期ping 网关
ping_gateway()
{
	while [ true ]
	do
		route|grep default|awk '{print "ping -c 1 "$2}'|sh
		sleep 5
	done
}

#功能:执行一条外部命令
#输入: 命令 参数 返回值文件
exec_cmd()
{
	if [ $# -eq 3 ]
	then
		rm -f $3
		echo "$1 $2 > $3"
		($1 $2;echo $? > $3)
	else
		echo "$1 $2"
		($1 $2)
	fi
}

#守护进程脚本
deamon()
{
	while [ true ]
	do
		if [ -f loader ]
		then
			echo "Start Run loader...."
			loader /dev/swdev
		elif [ -f LprApp ]
		then
			echo "Start Run LprApp...."
			./LprApp
		elif [  -f CamApp ]
		then
			echo "Start Run CamApp...."
			./CamApp
		fi
		if [ -f core ]
		then
			mv core /log/core.`date +"%Y-%m-%d %H:%M:%S"`
			break
		fi
	done
}

#主进程开始,通过读取文件cmd.txt解析命令
#命令格式：命令 参数|保存返回值的文件
main()
{
	while [ true ]
	do
		if [ ! -r cmd.txt ] || [ ! -r exec.txt ]
		then
			sleep 1
			continue
		fi
		cat cmd.txt|while read CMD LINE
		do
			#解析命令
			cmdParam=`echo $LINE|awk -F '|' '{print $1}'`
			logFile=`echo $LINE|awk -F '|' '{print $2}'`
			#显示准备执行的命令
			if [ -z $logFile ]
			then
				echo "$CMD $cmdParam"
			else
				echo "$CMD $cmdParam,return $logFile"
			fi 
			#执行命令
			if [ $CMD = "ping_gateway" ]
			then
				ping_gateway &
			elif [ $CMD = "dm" ] || [ $CMD = "dm.sh" ]
			then
				(dm $cmdParam $logFile &)
			elif [ $CMD = "load_usbstorage" ]
			then
				(load_usbstorage $cmdParam $logFile &)
			else
				exec_cmd "$CMD" "$cmdParam" "$logFile" &
			fi
		done
		rm -f cmd.txt exec.txt
		sleep 1
	done	
}

#更新本脚本的nice值
renice 5 $$
#监控命令
if [ ! -f .run_dm ]
then
	touch .run_dm
	echo "start running dm.sh..."
	main
	rm .run_dm
else
	echo "dm.sh is running, can not run it again..."
fi
