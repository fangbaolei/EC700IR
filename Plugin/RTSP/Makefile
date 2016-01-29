#编译选项
include ../../Makefile.mk
#CFLAG += -fpic 
#CXXFLAG += -fpic 

SRC_DIR = src
INC += \
  -I$(WORK_BASE)/Plugin/RTSP/inc \
  -I$(WORK_BASE)/Plugin/RTSP/src

LDFLAG += \
  -luuid

#目标文件
#PROGRAM=libRTSP.so
PROGRAM=libRTSP.a

#源文件
SRC=$(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp)
OBJ=$(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SRC))) 

#编译规则
all:$(PROGRAM)

#依赖关系
$(PROGRAM):$(OBJ)
	$(AR) -rc $@ $^
	mkdir -p $(LIBPATH)
	cp -vf $@ ../lib/
	mv $@ $(LIBPATH)/$@

clean:
	rm -Rf $(PROGRAM) $(LIBPATH)/$(PROGRAM) $(OBJ) $(SRC_DIR)/*.bak
