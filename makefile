#*******************************************************************************
#* FILE PURPOSE: Top level makefile for Creating Processor SDK Component
#*               Libraries and binaries
#*******************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Makefile for SDK level components
#*
#*
#*******************************************************************************
#*
# (Mandatory) Specify where various tools are installed.

ifndef MAKE
export MAKE = make
endif

ifndef ECHO
export ECHO = echo
endif

all: audio_demo
clean: audio_demo_clean

help:
	@echo "Standard Targets:"
	@echo "    help      - Prints target information"
	@echo "    all       - Builds all Component targets"
	@echo "    clean     - Cleans all Component targets"
	@echo ""
	@echo "Component Targets:"
	@echo "    audio_demo       - Builds audio equalization demo"
	@echo "    audio_demo_clean - Cleans audio equalization demo"
	@echo ""
	@echo "    NOTE: Instructions for rebuilding all other components"
	@echo "          installed with Processor SDK can be found in each"
	@echo "          component's sub-directory"

audio_demo:
ifeq ($(SOC),k2g)
	$(MAKE) -C ./evmK2G/build/make all
endif
ifeq ($(SOC),am57xx)
	$(MAKE) -C ./evmAM572x/build/make all
endif

audio_demo_clean:
ifeq ($(SOC),k2g)
	$(MAKE) -C ./evmK2G/build/make clean
endif
ifeq ($(SOC),am57xx)
	$(MAKE) -C ./evmAM572x/build/make clean
endif