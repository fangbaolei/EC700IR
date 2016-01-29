#编译选项
include ../../Makefile.mk
INC += -I$(WORK_BASE)/Algorithm/src/Inc
#目标文件
PROGRAM=libSWFC.a

#编译规则
all:$(PROGRAM)

#依赖关系
$(PROGRAM):$(OBJ)
	$(AR) -rc $@ $^
	mkdir -p $(LIBPATH)
	mv $@ $(LIBPATH)/$@

clean:
	rm -Rf $(PROGRAM) $(LIBPATH)/$(PROGRAM) $(OBJ) $(DEPENDFILE) *.bak
