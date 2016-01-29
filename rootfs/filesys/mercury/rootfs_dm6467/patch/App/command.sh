#!/bin/sh
#命令管理脚本，主要包含了一系列的函数，通过脚本调用具体的函数，调用方法如下
#command.sh 函数名 参数
#成功返回0，失败返回对应的错误代码

#功能:获得硬盘的大小
#返回值:硬盘大小
disk_size()
{
	if [ `disk_is_exist` -eq 0 ];then
		echo "0"
	fi
	fdisk -l| grep "Disk /dev/sd[a-z]"|awk '{print $3}'|awk -F . '{print $1}'
}

#功能: 硬盘是否存在
#返回值：硬盘个数
disk_is_exist()
{
	fdisk -l|wc -l
}

#功能:硬盘分区，如果硬盘还没有分区，则分区，否则直接格式化
#输入参数：$1:盘符;$2:分区数 
fdisk_disk()
{
	if [ `disk_is_exist` -eq 0 ];then
		return 1
	fi

	HDD=$1
	num=$2
	
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
	
	hasDiskNum=`fdisk -l|egrep "${HDD}[1-9]"|grep -v grep|wc -l`
	echo "start fdisk,hasDiskNum=$hasDiskNum..."
	#先删除现有的分区
	while [ $hasDiskNum -gt 0 ]
	do
	  hasDiskNum=`fdisk -l|egrep "${HDD}[1-9]"|grep -v grep|wc -l`
		echo "=========remove disk partition = $hasDiskNum============"
		if [ $hasDiskNum -gt 1 ]
		then
			fdisk ${HDD} <<-!
				d
				$hasDiskNum
				w
			!
		else
			fdisk ${HDD} <<-!
				d
				w
			!
		fi		
		hasDiskNum=`expr $hasDiskNum - 1`
	done
	#然后创建分区
	if [ `fdisk -l|grep MB|wc -l` -eq 1 ]
	then
		size=`echo $num \`disk_size\`|awk '{print "+"$2/$1"M"}'`
	else
	  size=`echo $num \`disk_size\`|awk '{print "+"$2/$1*1000"M"}'`
	fi

	if [ $num -gt 4 ]
	then
		num=`expr $num + 1`
	fi

	i=1
	while [ $i -le $num ]
	do
		echo "===========create disk partition $i,size =$size============="
		#最后一个分区
		if [ $i -eq $num ]
		then
			if [ $i -le 4 ]
			then
				fdisk ${HDD} <<-!
					n
					p
					$i
					
					
					w
				!
				if [ $? -ne 0 ];then
					return $?
				fi
			else
				fdisk ${HDD} <<-!
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
			fdisk ${HDD} <<-!
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
			fdisk ${HDD} <<-!
				n
				e
				
				
				w
			!
			if [ $? -ne 0 ];then
				return $?
			fi
		#大于4个分区的分区方式
		else
			fdisk ${HDD} <<-!
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
	partprobe	
	return 0	
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
	if [ $# -eq 1 ]
	then
		umount -f $1
		echo $?
	else
		df|grep "/dev/sd[a-z]"|while read DISK
		do
			umount $DISK
		done
	fi
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
			fdisk -l|grep Linux|awk '{print $1}'|while read DISK
			do
				badblocks -s -v /dev/${DISK}
			done
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

#硬盘管理函数
#$1 命令id
#$2 命令参数
disk_cmd()
{
	CMD=$1
	PARAM=$2
	echo "disk comamnd:$CMD, parameter:$PARAM"
	case $CMD in
		0)
			if [ ! -f ./.disk_cmd_${CMD}_running ]
			then
				touch ./.disk_cmd_${CMD}_running
				rm  -f disk_cmd_${CMD}.txt			
				echo "1" > disk_cmd_${CMD}.txt
				umount_disk
				ls /dev/sd[a-z]|while read DISK
				do
					fdisk_disk $DISK $PARAM
				done
				if [ $? -eq 0 ]
				then
					format_disk
					if [ $? -ne 0 ]
					then
						echo "3" > disk_cmd_${CMD}.txt
					fi
				else
					echo "3" > disk_cmd_${CMD}.txt
				fi			
				echo "2" > disk_cmd_${CMD}.txt
				rm -f ./.disk_cmd_${CMD}_running
			else
				echo "fdisk command is running."
			fi
		;;
	esac
}

#ntp时间同步
#$1 同步时间间隔，单位：秒
#$2 ntp服务器 
ntp_sync()
{
	PARAM=$@
	if [ ! -f ./.running_ntp ]
	then
		touch ./.running_ntp
		while [ -f ./.running_ntp ]
		do
			if [ -f ./ntp_param ]
			then
				PARAM=`cat ./ntp_param`
			fi
			echo "$PARAM"|while read TIME IP
			do
				echo "ntpdate -u -t 0.6 $IP"
				s=`date +"%s"`
				ntpdate -u -t 0.6 $IP
				e=`date +"%s"`
				if [ $? -eq 0 ]
				then
					if [ `expr $e - $s` -gt 3 ]
					then
						echo "hwclock -w --utc"
						hwclock -w --utc
					fi
					touch ./ntp_ok
				fi
				sleep $TIME
			done	
		done
		rm ./.running_ntp
	else
		echo $PARAM > ./ntp_param
	fi
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
  "fdisk_disk")  
  	(fdisk_disk $param)&
  	return 0
	;;
	
	"format_disk") 
		(format_disk $param)&
		return 0
	;;
	
	"check_disk")  return `check_disk $param`
	;;
	
	"mount_disk")  return `mount_disk $param`
	;;
	
	"umount_disk") return `umount_disk $param`
	;;
	
	"init_LprApp") return `init_LprApp`
	;;
	
	"disk_cmd")
		(disk_cmd $param) &
		return 0
	;;
	
	"ntp_sync") 
		(ntp_sync $param) &
		return 0
	;;
	
	*)
	(exec_cmd "$command $param"; return $?)
	;;
esac
