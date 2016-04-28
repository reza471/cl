#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/ProcSDK_K2G34/edma3_lld_02_12_01_23/packages;C:/ti/ProcSDK_K2G34/ipc_3_42_00_02/packages;C:/ti/ProcSDK_K2G34/bios_6_45_01_29/packages;C:/ti/ProcSDK_K2G34/pdk_k2g_1_0_1/packages;C:/ti/ProcSDK_K2G34/ndk_2_24_03_35/packages;C:/ti/ProcSDK_K2G34/uia_2_00_03_43/packages;C:/ti/ccsv6/ccs_base;C:/ti/ProcSDK_K2G34/pdk_k2g_1_0_1/packages/MyExampleProjects/MCASP_Audio_evmK2G_c66ExampleProject/.config
override XDCROOT = C:/ti/ProcSDK_K2G34/xdctools_3_32_00_06_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/ProcSDK_K2G34/edma3_lld_02_12_01_23/packages;C:/ti/ProcSDK_K2G34/ipc_3_42_00_02/packages;C:/ti/ProcSDK_K2G34/bios_6_45_01_29/packages;C:/ti/ProcSDK_K2G34/pdk_k2g_1_0_1/packages;C:/ti/ProcSDK_K2G34/ndk_2_24_03_35/packages;C:/ti/ProcSDK_K2G34/uia_2_00_03_43/packages;C:/ti/ccsv6/ccs_base;C:/ti/ProcSDK_K2G34/pdk_k2g_1_0_1/packages/MyExampleProjects/MCASP_Audio_evmK2G_c66ExampleProject/.config;C:/ti/ProcSDK_K2G34/xdctools_3_32_00_06_core/packages;..
HOSTOS = Windows
endif
