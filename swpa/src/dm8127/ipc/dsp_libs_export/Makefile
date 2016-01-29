#DSP_HOME := /sdk/rdk/IPNC_DM812x_IMX136_v3.8/ti_tools/cgt6x_7_4_4/bin
LNK      := ./bin/lnk6x
STRIP	 := ./bin/strip6x
LDFLAG   := --emit_warnings_as_errors\
						--diag_warning=10063-D\
						--display_error_number\
						--warn_sections\
						-q\
						--silicon_version=6740\
						-c\
						--dynamic\
						-x\
						--zero_init=off\
						--retain=_Ipc_ResetVector

ifdef DIR
	WORK_BASE=$(DIR)
else	
	WORK_BASE=$(shell pwd)
endif

ALG ?= normal

PROGRAM := $(WORK_BASE)/bin/ARM/ipnc_rdk_c6xdsp.xe674

CMD     := $(wildcard cmd/*.cmd)
LIB     := $(wildcard lib/*.ae674 lib/*.lib)
ALGLIB  := $(wildcard alg/$(ALG)/*.lib)
OBJ     := $(wildcard obj/*.oe674 obj/*.obj)
FILES   := $(CMD) $(LIB) $(ALGLIB) $(OBJ)
LNK_LIBS:= $(addprefix -l,$(LIB)) $(addprefix -l,$(ALGLIB))

all:$(PROGRAM)
	$(STRIP) $(PROGRAM)
	
ifeq ($(ALG),normal)	
	cp $(PROGRAM) ../../../../src/dm8127/ipc/firmware/ipnc_rdk_fw_c6xdsp.xe674
endif

ifeq ($(ALG),EP)
	cp $(PROGRAM) ../../../../src/dm8127/ipc/firmware/EP_dsp/ipnc_rdk_fw_c6xdsp.xe674
endif

ifeq ($(ALG),PK)
	cp $(PROGRAM) ../../../../src/dm8127/ipc/firmware/PK_dsp/ipnc_rdk_fw_c6xdsp.xe674
endif

ifeq ($(ALG),TrafficGate)
	cp $(PROGRAM) ../../../../src/dm8127/ipc/firmware/TrafficGate_dsp/ipnc_rdk_fw_c6xdsp.xe674
endif
$(PROGRAM):$(FILES)
	mkdir -p $(WORK_BASE)/bin/ARM
	$(LNK) -o $@ $(LDFLAG) $(LNK_LIBS) $(OBJ) $(CMD)
	
clean:
	rm -f $(PROGRAM)
