## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd package/cfg/audiosample_pe66.oe66

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/audiosample_pe66.xdl
	$(SED) 's"^\"\(package/cfg/audiosample_pe66cfg.cmd\)\"$""\"C:/ti/ProcSDK_K2G34/pdk_k2g_1_0_1/packages/MyExampleProjects/MCASP_Audio_evmK2G_c66ExampleProject/.config/xconfig_audiosample/\1\""' package/cfg/audiosample_pe66.xdl > $@
	-$(SETDATE) -r:max package/cfg/audiosample_pe66.h compiler.opt compiler.opt.defs
