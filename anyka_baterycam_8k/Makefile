# Project name
PROJ := sky39ev200


#source code Locations
export TOPDIR      = $(shell pwd)
export KERNELDIR   = $(TOPDIR)/kernel
export PLATFORMDIR = $(TOPDIR)/platform

export IMAGEPATH   = $(TOPDIR)/image

include $(KERNELDIR)/board.cfg
include $(TOPDIR)/system.cfg

DEFINE +=-DAK_RTOS
export DEFINE

#demo src
DEMODIR += $(PLATFORMDIR)/libapp/demo/ini
DEMODIR += $(PLATFORMDIR)/libapp/demo/dvr
DEMODIR += $(PLATFORMDIR)/libapp/demo/sysupd
DEMODIR += $(PLATFORMDIR)/libapp/demo/photo
DEMODIR += $(PLATFORMDIR)/libapp/demo/alarm
DEMODIR += $(PLATFORMDIR)/libapp/demo/version

ifeq ($(LWIP), y)
DEMODIR += $(PLATFORMDIR)/libapp/demo/net 
DEMODIR += $(PLATFORMDIR)/libapp/demo/uvc 
endif

ifeq ($(WIFI), y)
DEMODIR += $(PLATFORMDIR)/libapp/demo/wifi 
endif

ifeq ($(LIBAPP), y)
PLATLIBS += $(shell find $(PLATFORMDIR)/libapp/lib/ -name "*.a")
endif

#libakstreamenclib should be placed behand libmpi_venc for link order
ifeq ($(LIBMPI), y)
DEMODIR += $(PLATFORMDIR)/libmpi/demo 
PLATLIBS += $(shell find $(PLATFORMDIR)/libmpi/lib/ -name "*.a")
endif

ifeq ($(LIBPLAT), y)
DEMODIR  += $(PLATFORMDIR)/libplat/demo 
PLATLIBS += $(shell find $(PLATFORMDIR)/libplat/lib/ -name "*.a")
endif

#main application
ifeq ($(RING), y)
APPDIR += $(PLATFORMDIR)/apps/ring_call
endif
ifeq ($(AKIPC), y)
APPDIR += $(PLATFORMDIR)/apps/akipc
endif

APPDIR += $(PLATFORMDIR)/apps/main_ctrl
APPDIR += $(PLATFORMDIR)/apps/audio
APPDIR += $(PLATFORMDIR)/apps/video
APPDIR += $(PLATFORMDIR)/apps/spi_cmd
APPDIR += $(PLATFORMDIR)/apps/scheduler_layer
APPDIR += $(PLATFORMDIR)/apps/spi_layer
APPDIR += $(PLATFORMDIR)/apps/ringbuffer
APPDIR += $(PLATFORMDIR)/apps/fwupgrade
APPDIR += $(PLATFORMDIR)/apps/uploader
APPDIR += $(PLATFORMDIR)/apps/sdcard
APPDIR += $(PLATFORMDIR)/apps/p2p_sdcard_streaming
APPDIR += $(PLATFORMDIR)/apps/flv_recorder
ifeq ($(DEMO_SUPPORT), y)
APPDIR += $(DEMODIR)
endif

CSRCS += $(foreach d, $(APPDIR), $(wildcard $d/*.c))

APPOBJS += $(patsubst %.c, %.o, $(CSRCS))

#export for kernel use
export APPOBJS PLATLIBS


#build rule
.PHONY: all  apps kernel image  apps_clean clean 

all: prepare  apps kernel 

prepare:
ifneq ($(BOARD), cc3200bd)
ifneq ($(BOARD), devbd)
	$(error BOARD undefined!  Please use "make BOARD=cc3200bd" or "make BOARD=devbd" to make target on board with cc3200 or board with 8801 wifi)
endif
endif

	
kernel: prepare
	$(MAKE) -C $(TOPDIR)/kernel 

apps: prepare
	@for i in $(APPDIR); \
	do \
		$(MAKE) -C $$i ; \
		if [ $$? -ne 0 ]; then\
		  exit 1;\
		fi \
	done

apps_clean:
	for i in $(APPDIR); \
	do \
		$(MAKE) -C $$i  clean; \
	done

	
image:
	$(MAKE) -C $(TOPDIR)/kernel 

clean: apps_clean
	$(MAKE) -C $(TOPDIR)/kernel clean


#Rules
include $(TOPDIR)/rules.mk
