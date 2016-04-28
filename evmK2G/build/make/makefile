# set environment variables
CC = $(C6X_GEN_INSTALL_PATH)/bin/cl6x
HX = $(C6X_GEN_INSTALL_PATH)/bin/hex6x
SDK_SOC = $(SOC)
TIROOT := $(SDK_INSTALL_PATH)
PROC_SDK_INSTALL_PATH := $(PROC_SDK_INSTALL_PATH)
CG_TOOL_ROOT := $(TOOLCHAIN_PATH_A15)
XDC_INSTALL_BASE := $(XDC_INSTALL_PATH)
XDC_INSTALL_PATH := $(XDC_INSTALL_BASE)/packages
BIOS_INSTALL_PATH := $(BIOS_INSTALL_PATH)/packages
IPC_INSTALL_PATH := $(IPC_INSTALL_PATH)/packages
UIA_INSTALL_PATH := $(UIA_INSTALL_PATH)/packages
PDK_INSTALL_PATH_BASE := $(PDK_INSTALL_PATH)
PDK_INSTALL_PATH := $(PDK_INSTALL_PATH)
EDMA3LLD_BIOS6_INSTALLDIR := $(EDMA3LLD_BIOS6_INSTALLDIR)/packages


BOARD = evmK2G
DEVICE = K2G
XDC_PLATFORM := ti.platforms.evmTCI66AK2G02

CCS_BASE := $(TIROOT)/ccs6/ccs_base

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"configPkg/linker.cmd"

CC_OPTS  += -mv6600 --abi=eabi -g
LNK_OPTS += --reread_libs --warn_sections --display_error_number --diag_wrap=off --rom_model
DEFS += --define=SOC_K2G --define=USE_BIOS --define=DSP_MODE --define=IO_CONSOLE --define=MCASP_MASTER

DIAGS += --display_error_number --diag_suppress=1111 \
		 --diag_suppress=827 --diag_suppress=824 \
		 --diag_suppress=837 --diag_suppress=1037\
		 --diag_suppress=195 --diag_suppress=77 \
		 --diag_suppress=232 --diag_warning=225 \
		 --diag_warning=994 --diag_warning=262 --diag_wrap=off

INCS := --include_path="$(PDK_INSTALL_PATH_BASE)" \
		--include_path="$(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback/include" \
		--include_path="$(PDK_INSTALL_PATH)/ti/drv/mcasp/include" \
		--include_path="$(PDK_INSTALL_PATH)/ti/drv/mcasp" \
		--include_path="$(PDK_INSTALL_PATH)/ti/board" \
		--include_path="$(C6X_GEN_INSTALL_PATH)/include" \
		--include_path="$(UIA_INSTALL_PATH)" \
		--include_path="$(EDMA3LLD_BIOS6_INSTALLDIR)" \
		--include_path="$(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/filters"
        
ORDERED_OBJS += \
"./aic31_if.obj" \
"./AIC31.obj" \
"./audio_evmInit.obj" \
"./audioEQ_filtering.obj" \
"./audioEQ_main.obj" \
"./audioEQ_biquad.obj" \
"./codec_if.obj" \
"./i2c_soc.obj" \
"./UART_soc.obj" \
"./mcasp_soc.obj" \
"./mcasp_osal.obj" \
$(GEN_CMDS__FLAG) \
-l"libc.a" \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
###-include subdir_vars.mk
CFG_SRCS += \
$(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/evmK2G/src/audioEQ_demo.cfg 

C_SRCS += \
$(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback\src\aic31_if.c \
$(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback\src\AIC31.c \
$(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback\src\audio_evmInit.c \
$(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/evmK2G/src/audioEQ_filtering.c \
$(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/evmK2G/src/audioEQ_main.c \
$(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/evmK2G/src/audioEQ_biquad.c \
$(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback\src\codec_if.c \
$(PDK_INSTALL_PATH)/ti/drv/i2c/soc/k2g/i2c_soc.c \
$(PDK_INSTALL_PATH)/ti/drv/uart/soc/k2g/uart_soc.c \
$(PDK_INSTALL_PATH)/ti/drv/mcasp/soc/k2g/mcasp_soc.c \
$(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback\src\mcasp_osal.c

OBJS += \
./aic31_if.obj \
./AIC31.obj \
./audio_evmInit.obj \
./audioEQ_filtering.obj \
./audioEQ_main.obj \
./audioEQ_biquad.obj \
./codec_if.obj \
./i2c_soc.obj \
./UART_soc.obj \
./mcasp_soc.obj \
./mcasp_osal.obj \

C_DEPS += \
./aic31_if.pp \
./AIC31.pp \
./audio_evmInit.pp\
./audioEQ_filtering.pp \
./audioEQ_main.pp \
./audioEQ_biquad.pp \
./codec_if.pp \
./i2c_soc.pp \
./UART_soc.pp \
./mcasp_soc.pp \
./mcasp_osal.pp \

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_FILES__QUOTED += \
"configPkg/linker.cmd" \
"configPkg/compiler.opt" 

GEN_MISC_DIRS__QUOTED += \
"configPkg/" 

###-include subdir_rules.mk
configPkg/linker.cmd: $(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/evmK2G/src/audioEQ_demo.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"$(XDC_INSTALL_BASE)/xs" --xdcpath="$(EDMA3LLD_BIOS6_INSTALLDIR);$(BIOS_INSTALL_PATH);$(PDK_INSTALL_PATH);$(UIA_INSTALL_PATH);" xdc.tools.configuro -o configPkg -t ti.targets.elf.C66 -p $(XDC_PLATFORM) -r debug -c "$(C6X_GEN_INSTALL_PATH)" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: | configPkg/linker.cmd
configPkg/: | configPkg/linker.cmd

aic31_if.obj: $(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback/src/aic31_if.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)  $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="aic31_if.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

AIC31.obj: $(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback/src/AIC31.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)   $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="AIC31.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '
	
audio_evmInit.obj: $(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback/src/audio_evmInit.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)   $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="audio_evmInit.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

audioEQ_filtering.obj: $(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/evmK2G/src/audioEQ_filtering.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)  $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="audioEQ_filtering.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

audioEQ_main.obj: $(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/evmK2G/src/audioEQ_main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)  $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="audioEQ_main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

audioEQ_biquad.obj: $(PROC_SDK_INSTALL_PATH)/ti-design-sw/audio_equalization/filters/audioEQ_biquad.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)  $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="audioEQ_biquad.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

codec_if.obj: $(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback/src/codec_if.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)   $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="codec_if.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

i2c_soc.obj: $(PDK_INSTALL_PATH)/ti/drv/i2c/soc/k2g/i2c_soc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)   $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="i2c_soc.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '
	
UART_soc.obj: $(PDK_INSTALL_PATH)/ti/drv/uart/soc/k2g/UART_soc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)   $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="UART_soc.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

mcasp_soc.obj: $(PDK_INSTALL_PATH)/ti/drv/mcasp/soc/k2g/mcasp_soc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)   $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="mcasp_soc.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

mcasp_osal.obj: $(PDK_INSTALL_PATH)/ti/drv/mcasp/example/evmK2G/AIC31_Stereo_Loopback/src/mcasp_osal.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"$(C6X_GEN_INSTALL_PATH)/bin/cl6x" $(CC_OPTS) $(INCS)  $(DEFS) $(DIAGS) --preproc_with_compile --preproc_dependency="mcasp_osal.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
audioEQ_demo.out \

EXE_OUTPUTS__QUOTED += \
"audioEQ_demo.out" \

MAP_OUTPUTS += \
audioEQ_demo.map \

MAP_OUTPUTS__QUOTED += \
"audioEQ_demo.map" \


# All Target
all: audioEQ_demo.out

# Tool invocations
audioEQ_demo.out: $(OBJS) $(GEN_CMDS)
	@echo $(PDK_INSTALL_PATH)
	@echo $(PROCSDK_INSTALL_PATH)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	$(CC) $(CC_OPTS) $(DEFS) $(DIAGS) -z -m"audioEQ_demo.map" --heap_size=0x800 --stack_size=0x800 -i"$(C6X_GEN_INSTALL_PATH)/lib" $(LNK_OPTS) --xml_link_info="audioEQ_demo_linkInfo.xml" -o "audioEQ_demo.out" $(ORDERED_OBJS)
	cp -pf $@ ../../../bin/audioEQ_demo_evmK2G.out
	@echo 'Finished building target: $@'
	@echo ' '

audioEQ_demo.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C6000 Hex Utility'
	$(HX)  -o "audioEQ_demo.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(MAP_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) *.pp *.obj 
	-$(RM) "./audioEQ_demo_linkInfo.xml"
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
