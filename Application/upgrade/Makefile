#编译选项
include ../../Makefile.mk
#CFLAG += -DENABLE_AUTOTEST
#CXXFLAG += -DENABLE_AUTOTEST
INC+=-I$(WORK_BASE)/BasicModule/src/public/SWBaseLinkCtrl -I$(WORK_BASE)/swpa/src/$(M)/driver
ifeq ($(M),dm8127)
INC +=-I$(WORK_BASE)/Plugin/mtd/ubi-utils/include \
      -I$(WORK_BASE)/Plugin/mtd/include \
	  -I$(WORK_BASE)/BasicModule/inc \
	  -I$(WORK_BASE)/BasicModule/src/public/SWMatchTransformFilter/Device \
	  -I$(WORK_BASE)/swpa/src/dm8127/ipc/inc
LDFLAG += -L$(WORK_BASE)/Plugin/mtd/lib \
          -lmtd \
          -lscan \
          -lubi \
          -lubigen
endif

#目标文件
PROGRAM=UpgradeApp

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
