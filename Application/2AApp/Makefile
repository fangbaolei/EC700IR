#编译选项
include ../../Makefile.mk
#INC+=-I$(WORK_BASE)/Plugin/RTSP/inc
#目标文件
PROGRAM=2AApp

LDFLAG += -lsw2a -L./
#LDFLAG += -ldevice -L$(WORK_BASE)/swpa/src/dm8127/driver

#源文件
SRC=$(wildcard *.c *.cpp)
OBJ=$(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SRC))) 

#编译规则
all:$(PROGRAM)

#依赖关系
$(PROGRAM):$(OBJ) $(LIBS)
	$(CXX) -o $@ $^ $(LDFLAG)
	$(STRIP) $@
	mkdir -p $(BINPATH)
	mv $@ $(BINPATH)

clean:
	rm -Rf $(PROGRAM) $(BINPATH)/$(PROGRAM) $(OBJ) $(DEPENDFILE) *.bak
