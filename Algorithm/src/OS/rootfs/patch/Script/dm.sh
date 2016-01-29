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
	
	hasDiskNum=`fdisk -l|egrep "/dev/sda[1-9]"|grep -v grep|wc -l`
	echo "start fdisk,hasDiskNum=$hasDiskNum..."
	#先删除现有的分区
	while [ $hasDiskNum -gt 0 ]
	do
	  hasDiskNum=`fdisk -l|egrep "/dev/sda[1-9]"|grep -v grep|wc -l`
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
	if [ `fdisk -l|grep MB|wc -l` -eq 1 ]
	then
		size=`echo $num \`disk_size\`|awk '{print "+"$2/$1-100"M"}'`
	else
	  size=`echo $num \`disk_size\`|awk '{print "+"$2/$1*1000-100"M"}'`
	fi
	#最后剩余的空间给日志
	num=`expr $num + 1`
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
	num=0
	while [ `df|egrep "Result_disk|log"|wc -l` -ne 0 ] && [ $num -lt 3 ]
	do
			echo "retry umount disk $num times."
	    ps -ef|grep find|grep -v grep|awk '{print $1}'|xargs kill
	    df|egrep "Result_disk|log"|awk '{print $NF}'|while read LINE
	    do               
	    	echo "umount -f $LINE"
	    	umount -f $LINE
	    done
	    num=`expr $num + 1`
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
		return 0
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
	
	if [ $number -eq 0 ]
	then
		number=`fdisk -l|grep Linux|wc -l|awk '{print $1-1}'`
	fi
	
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
			while [ `ps -ef|egrep "fdisk|mkfs.ext3|e2fsck"|grep -v grep|wc -l` -ne 0 ]
			do
				ps -ef|egrep "fdisk|mkfs.ext3|e2fsck"|grep -v grep|xargs kill
				sleep 1
			done
			umount_disk
 			fdisk_disk $number
			if [ $? -ne 0 ];then
				echo 1 > $logFile
				return
			fi
			format_disk
			if [ $? -ne 0 ];then
				echo 1 > $logFile
				return
			fi
			mount_disk $dir
			if [ $? -ne 0 ];then
				echo 1 > $logFile
				return
			fi
			ret=`df|grep Result_disk|grep -v log|grep -v grep|wc -l`
			echo "df|grep Result_disk|grep -v log|grep -v grep|wc -l=$ret, number=$number"
			if [ $ret -eq $number ] && [ $ret -ne 0 ]
			then
				echo 0 > $logFile
			else
				echo 1 > $logFile
			fi
		;;
		2)
			while [ `ps -ef|egrep "fdisk|mkfs.ext3|e2fsck"|grep -v grep|wc -l` -ne 0 ]
			do
				ps -ef|egrep "fdisk|mkfs.ext3|e2fsck"|grep -v grep|xargs kill
				sleep 1
			done
			umount_disk
			check_disk $logFile
			if [ $? -ne 0 ];then
				echo 1 > $logFile
				return
			fi
			mount_disk $dir
			if [ $? -ne 0 ];then
				echo 1 > $logFile
				return
			fi
			ret=`df|grep Result_disk|grep -v log|grep -v grep|wc -l`
			echo "df|grep Result_disk|grep -v log|grep -v grep|wc -l=$ret, number=$number"
			if [ $ret -eq $number ] && [ $ret -ne 0 ]
			then
				echo 0 > $logFile
			else
				echo 1 > $logFile
			fi
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
			ret=`df|grep Result_disk|grep -v log|grep -v grep|wc -l`
			echo "df|grep Result_disk|grep -v log|grep -v grep|wc -l=$ret, number=$number"
			if [ $ret -eq $number ] && [ $ret -ne 0 ]
			then
				echo 0 > $logFile
			else
				echo 1 > $logFile
			fi
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
			ret=`df|grep Result_disk|grep -v log|grep -v grep|wc -l`
			echo "df|grep Result_disk|grep -v log|grep -v grep|wc -l=$ret, number=$number"
			if [ $ret -eq $number ] && [ $ret -ne 0 ]
			then
				echo 0 > $logFile
			else
				echo 1 > $logFile
			fi
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
			ret=`df|grep Result_disk|grep -v log|grep -v grep|wc -l`
			echo "df|grep Result_disk|grep -v log|grep -v grep|wc -l=$ret, number=$number"
			if [ $ret -eq $number ] && [ $ret -ne 0 ]
			then
				echo 0 > $logFile
			else
				echo 1 > $logFile
			fi
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

#删除目录
delete_directory()
{
	#要删除的目录
	dir=$1
	me=$2
	if [ ! -d $dir ]
	then
		return
	fi
	num=`basename $dir`
	if [ -z $num ] || [ $num = "file" ] || [ $num = "data" ]
	then
		echo "$dir is the top directory,can not delete."
		return
	fi
	#删除自己的目录
	if [ $me -eq 1 ]
	then
		dir=`dirname $dir`
		#删除小于等于当前索引的目录
		while [ $num -ge 0 ]
		do
			if [ -d $dir/$num ]
			then
				echo "delete overwrite directory:rm -Rf $dir/$num"
				rm -Rf $dir/$num
			elif [ -d $dir/0$num ]
			then
				echo "delete overwrite directory:rm -Rf $dir/0$num"
				rm -Rf $dir/0$num
			fi
			num=`expr $num - 1`
		done
		#若目录为空，则删除
		if [ `(rmdir -p $dir 2>/dev/null;echo $?)` -eq 0 ]
		then
			echo "delete empty directory:rmdir -p $dir"
		fi
	else		
		#删除比自己小的目录
		while [ ! -z $num ] && [ $num  != "file" ] && [ $num  != "data" ]
		do
			dir=`dirname $dir`
			ls $dir|while read DIR
			do
				if [ $DIR -lt $num ]
				then
					while [ `ps -ef|grep "rm -Rf $dir/$DIR"|grep -v grep|wc -l` -ne 0 ]
					do
						sleep 1
					done
					echo "delete invalid directory:rm -Rf $dir/$DIR"
					rm -Rf $dir/$DIR
				fi	
			done
			num=`basename $dir`
		done
	fi	
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
	times=0
	while [ true ]
	do
		#清除1天之前的txt文件
		if [ $times -ge 86400 ]
		then			
			find /usr/local/signalway -name '*.txt' -mtime +1|xargs rm -f
			times=0
		fi
		
		if [ ! -r cmd.txt ] || [ ! -r exec.txt ]
		then
			sleep 1
			times=`expr $times + 1`
			continue
		fi

		#遍历文件
		cat cmd.txt|while read CMD LINE
		do
			#解析命令
			cmdParam=`echo $LINE|awk -F '|' '{print $1}'`
			logFile=`echo $LINE|awk -F '|' '{print $2}'`
			#显示准备执行的命令
			if [ -z $logFile ]
			then
				logFile=`echo $CMD|awk -F '|' '{print $2}'`
				CMD=`echo $CMD|awk -F '|' '{print $1}'`
				if [ -z $logFile ]
				then
					echo "$CMD"
				else
					echo "$CMD,return $logFile"
				fi
			else
				echo "$CMD $cmdParam,return $logFile"
			fi 
			#执行命令
			if [ $CMD = "ping_gateway" ]
			then
				ping_gateway &
			elif [ $CMD = "dm" ] || [ $CMD = "dm.sh" ]
			then
				(dm $cmdParam $logFile) &
			elif [ $CMD = "RM" ] || [ $CMD = "rm" ]
			then
				(delete_directory $cmdParam;echo "0" > "$logFile") &
			else
				(exec_cmd "$CMD" "$cmdParam" "$logFile") &
			fi
		done
		rm -f cmd.txt exec.txt
		sleep 1
		times=`expr $times + 1`
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
	echo "stop running dm.sh..."
	rm .run_dm
else
	echo "dm.sh is running, can not run it again..."
fi
