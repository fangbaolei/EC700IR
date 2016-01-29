#编译选项
include ../../Makefile.mk
INC+=-I$(WORK_BASE)/BasicModule/src/public/SWBaseLinkCtrl -I$(WORK_BASE)/Algorithm/src/Inc
LDFLAG += -L$(WORK_BASE)/Plugin/FreeType -lfreetype
LDFLAG += -L$(WORK_BASE)/BasicModule/lib -lsvEPDetApi.DM8127 -lsvTLDetApi.DM8127 -lsvTgIrDetApi.DM8127 -lsvDet.DM8127 -lsvLPRCore.DM8127 -lsvBase.DM8127 
#目标文件
PROGRAM=LprApp

LIBS+= $(wildcard $(WORK_BASE)/Plugin/FreeType/*.a)
LIBS+= $(wildcard $(WORK_BASE)/BasicModule/lib/*.a)

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
