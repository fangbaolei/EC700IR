#编译选项
include ../../Makefile.mk
INC+=-Iinclude
#目标文件
PROGRAM=GB28181App

LDFLAG += -leXosip2 -losip2  -losipparser2  -lresolv

#源文件
SRC=$(wildcard *.c *.cpp)
OBJ=$(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SRC))) 

#编译规则
all:$(PROGRAM)

#依赖关系
$(PROGRAM):$(OBJ) $(LIBS)
	mkdir -p $(LIBPATH)
	cp $(WORK_BASE)/Application/GB28181App/lib/* $(LIBPATH)/
	$(CXX) -o $@ $^ $(LDFLAG)
	$(STRIP) $@
	mkdir -p $(BINPATH)
	mv $@ $(BINPATH)


clean:
	rm -Rf $(PROGRAM) $(BINPATH)/$(PROGRAM) $(OBJ) *.bak
