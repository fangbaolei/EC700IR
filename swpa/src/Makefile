#编译选项
include ../../Makefile.mk

#程序名
PROGRAM = libswpa_$(M).a
	
#CFLAG += -DSWPA_IPNC
#CFLAG += -DSWPA_CAMERA\
         -DSWPA_FILE\
         -DSWPA_COMMAND\
         -DSWPA_DATETIME\
         -DSWPA_DEVICE\
         -DSWPA_DMA\
         -DSWPA_HDD\
         -DSWPA_SEM\
         -DSWPA_SOCKET\
         -DSWPA_TCP_PRINT\
         -DSWPA_THREAD\
         -DSWPA_TIMER\
         -DSWPA_UTILS\
         -DSWPA_VPIF\
         

#编译规则
all:$(PROGRAM)

#依赖关系
$(PROGRAM):$(OBJ)
	@find . -name '*.a'|grep $(M)|grep -v grep|while read LINE;\
	do\
		echo "extract $$LINE...";\
		$(AR) -xv $$LINE >/dev/null;\
	done;		
	$(AR) -rc $@ $(OBJ) *.o
	rm *.o
	mkdir -p $(LIBPATH)
	mv $@ $(LIBPATH)/$@

clean:
	rm -Rf $(PROGRAM) $(LIBPATH)/$(PROGRAM) $(OBJ) $(DEPENDFILE) *.o *.bak
