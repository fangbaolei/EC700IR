#!/bin/sh
INC_FILE=DspBuildNo.h
BUILD_NO=`svn info|cat -n|awk '{if($1==5)print $3}'`

echo "#ifndef _DSP_BUILD_NO_"                          |tee    $INC_FILE
echo "#define _DSP_BUILD_NO_"                          |tee -a $INC_FILE
echo "const char *PSZ_DSP_BUILD_NO = \"1.1.0.$BUILD_NO\";"  |tee -a $INC_FILE
echo "const char *DSP_BUILD_DATE = __DATE__;"          |tee -a $INC_FILE
echo "const char *DSP_BUILD_TIME = __TIME__;"          |tee -a $INC_FILE
echo "#endif"                                          |tee -a $INC_FILE
echo ""                                                |tee -a $INC_FILE

