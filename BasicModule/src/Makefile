#编译选项
include ../../Makefile.mk

#头文件
INC += -I$(WORK_BASE)/Algorithm/src/Inc
INC += -I$(WORK_BASE)/BasicModule/inc/inc_det
INC += -I$(WORK_BASE)/BasicModule/inc/inc_det

#for jupiter
INC += -I$(WORK_BASE)/swpa/src/dm8127/ipc/inc
INC += -I$(WORK_BASE)/BasicModule/src/dm8127/SWDomeCameraControlMSG

#根据宏定义选择源文件
ifndef M
	PROGRAM=libFilter.a
else
ifeq ($(M),dm6467)
	PROGRAM=libFilter_dm6467.a
endif
		
ifeq ($(M),dm8127)
	PROGRAM=libFilter_dm8127.a
endif
endif


#编译规则
all:$(PROGRAM)

#依赖关系
$(PROGRAM):$(OBJ)
	$(AR) -rc $@ $^
	mkdir -p $(LIBPATH)
	mv $@ $(LIBPATH)/$@

clean:
	rm -Rf $(PROGRAM) $(LIBPATH)/$(PROGRAM) $(OBJ) $(DEPENDFILE) *.bak
