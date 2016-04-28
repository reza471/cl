## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd package/cfg/audioEQ_demo_pe66.oe66

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/audioEQ_demo_pe66.xdl
	$(SED) 's"^\"\(package/cfg/audioEQ_demo_pe66cfg.cmd\)\"$""\"C:/ti/ProcSDK202_Alpha/processor_sdk_rtos_am57xx_2_00_02_04/demos/audio_equalization/evmAM572x/build/ccs/.config/xconfig_audioEQ_demo/\1\""' package/cfg/audioEQ_demo_pe66.xdl > $@
	-$(SETDATE) -r:max package/cfg/audioEQ_demo_pe66.h compiler.opt compiler.opt.defs
