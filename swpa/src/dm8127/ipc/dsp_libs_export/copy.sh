#!/bin/sh

DST_DIR=`dirname $0`

copy_file()
{
	while [ $# -gt 0 ]
	do
		if [ -f $1 ]
		then		
			FNAME=`basename $1`
			EXT=`basename $1|awk -F '.' '{print $NF}'`
			if [ $EXT = "cmd" ]
			then			
				if [ ! -d $DST_DIR/cmd ]
				then
					mkdir $DST_DIR/cmd
				fi
				CMDFILE="$DST_DIR/cmd/$FNAME"
				echo "genate $CMDFILE"
				echo "" > $CMDFILE
				cat $1|while read LINE
				do
					if [ `echo $LINE|grep "\-l"|wc -l` -eq 0 ]
					then
						echo "$LINE" >> $CMDFILE
					else
						LIBNAME=`echo "$LINE"|grep "\-l"|cut -c 3-|tr -d '"'`
						copy_file $LIBNAME
						
						LIBNAME=`basename $LIBNAME`
						echo "-l"\"lib/$LIBNAME\" >> $CMDFILE
					fi
				done
			elif [ $EXT = "oe674" ]
			then
				echo "$FNAME==>$DST_DIR/obj/$FNAME"
				if [ ! -d $DST_DIR/obj ]
				then
					mkdir $DST_DIR/obj
				fi
				cp $1 $DST_DIR/obj
			else
				echo "$FNAME==>$DST_DIR/lib/$FNAME"
				if [ ! -d $DST_DIR/lib ]
				then
					mkdir $DST_DIR/lib
				fi
				cp $1 $DST_DIR/lib
			fi		
		fi
		shift
	done
}
echo "========================"
echo "copy file..."
shift
copy_file $*
echo "done."
echo "========================"