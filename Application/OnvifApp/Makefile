#编译选项
include ../../Makefile.mk
INC+=-I$(WORK_BASE)/Plugin/ONVIF/inc
#目标文件
PROGRAM=OnvifApp

LDFLAG += -lONVIF -luuid

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
	rm -Rf $(PROGRAM) $(BINPATH)/$(PROGRAM) $(OBJ) *.bak
