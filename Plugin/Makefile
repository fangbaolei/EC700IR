#编译选项
include ../Makefile.mk
INC += -I$(WORK_BASE)/Algorithm/src/Inc
#目标文件
PROGRAM=libPlugin.a

#源文件
SRC=$(wildcard tinyxml/*.c tinyxml/*.cpp FreeType/*.c FreeType/*.cpp qftp/*.c qftp/*.cpp)
OBJ=$(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SRC))) 

#编译规则
all:$(PROGRAM)

#依赖关系
$(PROGRAM):$(OBJ)
	$(AR) -rc $@ $^
	mkdir -p $(LIBPATH)
	cp -vf $@ ./lib/
	mv $@ $(LIBPATH)/$@
	cp -vf libjpeg/lib/libjpeg.a ./lib/

clean:
	rm -Rf $(PROGRAM) $(LIBPATH)/$(PROGRAM) $(OBJ) *.bak
