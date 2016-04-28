#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/ProcSDK202_Alpha/edma3_lld_02_12_01_23/packages;C:/ti/ProcSDK202_Alpha/ipc_3_42_00_02/packages;C:/ti/ProcSDK202_Alpha/bios_6_45_01_29/packages;C:/ti/ProcSDK202_Alpha/pdk_am57xx_1_0_2/packages;C:/ti/ProcSDK202_Alpha/ndk_2_24_03_35/packages;C:/ti/ProcSDK202_Alpha/uia_2_00_03_43/packages;C:/ti/ccsv6/ccs_base;C:/ti/ProcSDK202_Alpha/processor_sdk_rtos_am57xx_2_00_02_04/demos/audio_equalization/evmAM572x/build/ccs/.config
override XDCROOT = C:/ti/ProcSDK202_Alpha/xdctools_3_32_00_06_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/ProcSDK202_Alpha/edma3_lld_02_12_01_23/packages;C:/ti/ProcSDK202_Alpha/ipc_3_42_00_02/packages;C:/ti/ProcSDK202_Alpha/bios_6_45_01_29/packages;C:/ti/ProcSDK202_Alpha/pdk_am57xx_1_0_2/packages;C:/ti/ProcSDK202_Alpha/ndk_2_24_03_35/packages;C:/ti/ProcSDK202_Alpha/uia_2_00_03_43/packages;C:/ti/ccsv6/ccs_base;C:/ti/ProcSDK202_Alpha/processor_sdk_rtos_am57xx_2_00_02_04/demos/audio_equalization/evmAM572x/build/ccs/.config;C:/ti/ProcSDK202_Alpha/xdctools_3_32_00_06_core/packages;..
HOSTOS = Windows
endif
