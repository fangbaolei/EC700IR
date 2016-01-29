#!/bin/sh

add_blank_at_end_of_file()
{
	fname=$1
	if [ -f $fname ] && [ -n "`tail -n 1 $fname`" ]
	then
		echo "add a blank to the end of $fname"
		echo "" >> $fname
	fi
	
	if [ `file $fname|grep "UTF-8"|wc -l` -eq 1 ]
	then
		echo "iconv -f UTF-8 -t GB2312 $fname > $$"
		iconv -f UTF-8 -t GB2312 $fname > $$
		if [ $? -eq 0 ]
		then
			mv $$ $fname
		else
		  echo "iconv -f UTF-8 -t GB2312 $fname error"
		fi
	fi
}

find . -name '*.h'|grep -v SlaveApp|grep -v Kernel|grep -v DriverLib|grep -v BootLoader|grep -v Algorithm|while read LINE
do
	add_blank_at_end_of_file $LINE
done

find . -name '*.c'|grep -v SlaveApp|grep -v Kernel|grep -v DriverLib|grep -v BootLoader|grep -v Algorithm|while read LINE
do
	add_blank_at_end_of_file $LINE
done

find . -name '*.cpp'|grep -v SlaveApp|grep -v Kernel|grep -v DriverLib|grep -v BootLoader|grep -v Algorithm|while read LINE
do
	add_blank_at_end_of_file $LINE
done
