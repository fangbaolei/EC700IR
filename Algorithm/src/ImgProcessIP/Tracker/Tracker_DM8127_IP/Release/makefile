################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/c6000_7.4.1

ORDERED_OBJS += \
"./trafficlight_recognizer.obj" \
"./scalespeedimpl.obj" \
"./findBlobs.obj" \
"./TrafficLightInfo.obj" \
"./TrafficLightImpl.obj" \
"./TrackerImplHigh_PhotoRecog.obj" \
"./TrackerImplHigh.obj" \
"./TrackInfoHigh.obj" \
"./TrackImplPhoto.obj" \
"./SwCore.obj" \
"./ObjectTrackingInfo.obj" \
"./HVCAMdelay.obj" \
"./EPAppUtils.obj" \
"./CarDet.obj" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ../../../../../_lib/8127/Release/Tracker.lib

# Tool invocations
../../../../../_lib/8127/Release/Tracker.lib: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Archiver'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.1/bin/ar6x" r "../../../../../_lib/8127/Release/Tracker.lib" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_LIBRARY_OUTPUTS__QUOTED) "..\..\..\..\..\_lib\8127\Release\Tracker.lib"
	-$(RM) "CarDet.pp" "EPAppUtils.pp" "ObjectTrackingInfo.pp" "SwCore.pp" "TrackImplPhoto.pp" "TrackInfoHigh.pp" "TrackerImplHigh.pp" "TrackerImplHigh_PhotoRecog.pp" "TrafficLightImpl.pp" "TrafficLightInfo.pp" "findBlobs.pp" "scalespeedimpl.pp" "trafficlight_recognizer.pp" 
	-$(RM) "CarDet.obj" "EPAppUtils.obj" "HVCAMdelay.obj" "ObjectTrackingInfo.obj" "SwCore.obj" "TrackImplPhoto.obj" "TrackInfoHigh.obj" "TrackerImplHigh.obj" "TrackerImplHigh_PhotoRecog.obj" "TrafficLightImpl.obj" "TrafficLightInfo.obj" "findBlobs.obj" "scalespeedimpl.obj" "trafficlight_recognizer.obj" 
	-$(RM) "HVCAMdelay.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
