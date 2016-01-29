#参数
m ?= dm8127
t ?= 1
s ?= all
alg ?= TrafficGate

ifdef d
	USER_VERSION="USER_VERSION=-DUSER_VERSION=\\\"$d\\\""
endif

#环境变量
WORK_BASE = $(shell pwd)

ifeq ($(m),dm6467)
	APPPATH = $(WORK_BASE)/rootfs/filesys/venus/App	
endif

ifeq ($(m),dm8127)
	APPPATH = $(WORK_BASE)/rootfs/filesys/venus/App	
endif

ifeq ($(t),1)
	xml=LprApp.xml
	DSP=MercuryDsp.out
endif

ifeq ($(t),2)
	xml=LprApp_CapFace.xml
	DSP=MercuryDsp.out
endif

ifeq ($(t),3)
	xml=LprApp_TollGate.xml
	DSP=MercuryDsp.out
endif

ifeq ($(t),4)
	xml=LprApp_PT.xml
	DSP=MercuryDspPT.out
endif

#编译选项
help:
	@echo "Usage: make module_name [m=dm6467] [v=0] [t=1] [d=v1] s=[clean, all]"
	@echo "module_name:"
	@echo "    --- swpa      compile swpa"
	@echo "    --- SWFC      compile SWFC"
	@echo "    --- Plugin    compile Plugin"
	@echo "    --- ONVIF     compile ONVIF"
	@echo "    --- RTSP      compile RTSP"
	@echo "    --- bm        compile BasicModule"
	@echo "    --- LprApp    compile LprApp"
	@echo "    --- deamon    compile deamon"
	@echo "    --- command   compile command"
	@echo "    --- log       compile log"
	@echo "    --- upgrade   compile upgrade"
	@echo "    --- RtspApp   compile RtspApp"
	@echo "    --- ShmTool   compile ShmTool"
	@echo "    --- OnvifApp  compile OnvifApp"
	@echo "    --- GB28181App  compile GB28181App"
	@echo "    --- 2AApp  	 compile 2AApp"
	@echo "    --- dsp       link dsp program,alg=[normal,EP,PK,TrafficGate] default is normal. \"normal\" is for CapFace and TollGate, \"EP\" for EPolice, \"PK\" for Parking"
	@echo "    --- lib       compile all lib"
	@echo "    --- exe       compile all exe"
	@echo "    --- all       compile all projects"
	@echo "    --- rootfs    generate upgrade package"
	@echo "    --- install   complie all projects and generate upgrade package"
	@echo "    --- svn       download files from svn."
	@echo "    --- clean     clear all *.o and *.bak and *.d files"
	@echo "m:"
	@echo "    --- dm6467 compile dm6467 lib. default value"
	@echo "    --- dm8127 compile dm8127 lib"
	@echo "v:"
	@echo "    --- 0 disable verbose compile line. default value"
	@echo "    --- 1 enable verbose compile line."
	@echo "t:"
	@echo "    --- 1 EPoliceApp    package. default value"
	@echo "    --- 2 CapFaceApp    package"
	@echo "    --- 3 TollGateApp   package"
	@echo "    --- 4 TollGatePTApp package"
	@echo "d:"
	@echo "    --- user define version sufix."
	@echo "s:"
	@echo "    --- make session, clean or all"
	@echo "Exam:"
	@echo "      make swpa"
	@echo "      make swpa m=dm6467"
	@echo "      make swpa m=dm6467 v=1"
	@echo "      make swpa m=dm6467 v=1 d=v1"
	
swpa:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/swpa/src clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/swpa/src all
endif
	
SWFC:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/SWFC/src clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/SWFC/src all
endif	

Plugin:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Plugin clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Plugin all
endif
	
ONVIF:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Plugin/ONVIF clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Plugin/ONVIF all
endif	

RTSP:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Plugin/RTSP clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Plugin/RTSP all
endif	

bm:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/BasicModule/src clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/BasicModule/src all
endif
	
LprApp:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/LprApp clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/LprApp all
endif
	
deamon:	
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/deamon clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/deamon all
endif
	
command:	
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/command clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/command all
endif
	
log:	
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/log clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/log all	
endif
	
upgrade:
ifeq ($(s),clean)	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/upgrade clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/upgrade all		
endif
	
RtspApp:	
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/RtspApp clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/RtspApp all		
endif

ShmTool:	
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/ShmTool clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/ShmTool all		
endif
	
OnvifApp:	
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/OnvifApp clean
else	
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/OnvifApp all	
endif

GB28181App:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/GB28181App clean
else
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/GB28181App all	
endif

2AApp:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/2AApp clean
else
	$(MAKE) DIR=$(WORK_BASE) V=$(v) M=$(m) $(USER_VERSION) -C $(WORK_BASE)/Application/2AApp all	
endif

dsp:
ifeq ($(s),clean)
	$(MAKE) DIR=$(WORK_BASE) ALG=$(alg) -C $(WORK_BASE)/swpa/src/dm8127/ipc/dsp_libs_export clean	
else
	$(MAKE) DIR=$(WORK_BASE) ALG=$(alg) -C $(WORK_BASE)/swpa/src/dm8127/ipc/dsp_libs_export all
endif

svn:
	@echo "start download files."	
	@echo "sorry,function not compliment."
	
clean:
	@echo "clear all *.o and *.bak and *.d file"
	@find . -iname '*.o' -o -iname '*.bak' -o -iname '*.d'|xargs rm	
	@echo "done."	

cleansw:
	@echo "clean sw libs and exe."
	$(MAKE) lib s=clean
	$(MAKE) exe s=clean
	
rootfs:
	@echo "copy file to system..."
	@./copybin.sh
	
lib:swpa SWFC bm
	cp -vf ./Plugin/lib/*.a ./lib/ARM/

exe:LprApp deamon command log upgrade RtspApp OnvifApp GB28181App 2AApp ShmTool
all:lib exe
	
install:all rootfs

.PHONY:help\
			 swpa\
			 SWFC\
			 Plugin\
			 ONVIF\
			 RTSP\
			 bm\
			 LprApp\
			 deamon\
			 command\
			 log\
			 upgrade\
			 RtspApp\
			 ShmTool\
			 OnvifApp\
			 GB28181App\
			 2AApp\
			 dsp\
			 lib\
			 exe\
			 all\
			 rootfs\
			 install\
			 svn\
			 clean