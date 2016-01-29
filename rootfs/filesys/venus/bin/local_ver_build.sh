# file name: local_ver_build.sh
# log 修改git和svn判断条件，适应1.6、1.8版本

#!/bin/sh

if [ $# != 2 ]
then
	INC_DIR=.
	FILENAME=firmware_ver
else
	INC_DIR=$1
	FILENAME=$2
fi

VER_FILE=$INC_DIR/$FILENAME

# 读取Git版本号
TESTGIT=`git rev-list HEAD`
if [ "$TESTGIT" != "" ]
then
GITLOCALVER=`git rev-list HEAD | wc -l | awk '{print $1}'`
echo "Git Local version:" $GITLOCALVER
GIT_VER=r$GITLOCALVER
GIT_VER="Git: $GIT_VER $(git rev-list HEAD -n 1 | cut -c 1-7)"
GIT_VERSION=$GIT_VER
VB_HASGITVER=1
else
echo "There is no Git version control."
VB_HASGITVER=0
fi

#读取SVN版本号

#　根据不同的SVN版本得到仓库版本号
VER16=`svn --version | grep "1\.6"`
VER17=`svn --version | grep "1\.7"`
VER18=`svn --version | grep "1\.8"`

if [ "$VER16" != "" ]; then
    SVNLOCALVER=`svn info | cat -n | awk '{if($1==5)print $3}'`
    echo "1.6 version"
fi
if [ "$VER17" != "" ]; then
    SVNLOCALVER=`svn info | cat -n | awk '{if($1==6)print $3}'`
    echo "1.7 version"
fi
if [ "$VER18" != "" ]; then
    SVNLOCALVER=`svn info | cat -n | awk '{if($1==7)print $3}'`
    echo "1.8 version"
fi
if [ "$SVNLOCALVER" != "" ]
then
echo "SVN Local Version:" $SVNLOCALVER
SVN_VER=r$SVNLOCALVER
SVN_VER="$SVN_VER"
SVN_VERSION=$SVN_VER
VB_HASSVNVER=1
else
echo "There is no SVN version control."
VB_HASSVNVER=0
fi

#生成版本信息文件
if [ $VB_HASGITVER = 0 ] && [ $VB_HASSVNVER = 0 ]
then
echo "There isn't any version control."
else
echo "Generated:" $VER_FILE 

if [ $VB_HASGITVER = 1 ] && [ $VB_HASSVNVER = 0 ]
then
echo "Git version add."
echo "$GIT_VERSION" > $VER_FILE
fi

if [ $VB_HASGITVER = 0 ] && [ $VB_HASSVNVER = 1 ]
then
echo "SVN version add."
echo "$SVN_VERSION" > $VER_FILE
fi

if [ $VB_HASGITVER = 1 ] && [ $VB_HASSVNVER = 1 ]
then
echo "Git&SVN version both there, but only need SVN."
echo "$SVN_VERSION" > $VER_FILE
fi

sed '/^$/d' $VER_FILE > tmp_txt
mv tmp_txt $VER_FILE

fi
