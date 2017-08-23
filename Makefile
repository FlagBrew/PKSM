# TARGET #

TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif
endif

# COMMON CONFIGURATION #

PKSM=1
PKSV=0 
ROSALINA=0
DEBUG=0

ifeq ($(PKSV), 1)
	NAME := PKSV
else
	NAME := PKSM
endif

BUILD_BASE := build
OUTPUT_BASE := output

ifeq ($(PKSV), 1)
	ifeq ($(ROSALINA), 1)
		BUILD_DIR := build/rosalina/PKSV
	else
		BUILD_DIR := build/PKSV
	endif
else
	ifeq ($(DEBUG), 1)
		BUILD_DIR := build/PKSM/citra
	else
		ifeq ($(ROSALINA), 1)
			BUILD_DIR := build/rosalina/PKSM
		else
			BUILD_DIR := build/PKSM
		endif
	endif
endif

ifeq ($(PKSV), 1)
	ifeq ($(ROSALINA), 1)
		OUTPUT_DIR := output/rosalina/PKSV
	else
		OUTPUT_DIR := output/PKSV
	endif
else
	ifeq ($(DEBUG), 1)
		OUTPUT_DIR := output/PKSM/citra
	else
		ifeq ($(ROSALINA), 1)
			OUTPUT_DIR := output/rosalina/PKSM
		else
			OUTPUT_DIR := output/PKSM
		endif
	endif
endif

INCLUDE_DIRS :=
SOURCE_DIRS := source/memecrypto/source source/pp2d/pp2d source

EXTRA_OUTPUT_FILES :=

VERSION_MAJOR := 4
VERSION_MINOR := 5
VERSION_MICRO := 0

PORTLIBS_PATH := $(DEVKITPRO)/portlibs
PORTLIBS := $(PORTLIBS_PATH)/armv6k $(PORTLIBS_PATH)/3ds
CTRULIB ?= $(DEVKITPRO)/libctru

LIBRARY_DIRS := $(PORTLIBS) $(CTRULIB)
LIBRARIES := citro3d ctru m

BUILD_FLAGS := -march=armv6k -mtune=mpcore -mfloat-abi=hard
BUILD_FLAGS_CC := -g -Wall -Og -mword-relocations \
			-fomit-frame-pointer -ffunction-sections -ffast-math \
			$(BUILD_FLAGS) $(INCLUDE) -DARM11 -D_3DS \
			-DBUILDTOOLS \
			-DPKSV=${PKSV} \
			-DROSALINA_3DSX=${ROSALINA} \
			-DCITRA=${DEBUG} \
			-DDEBUG=${DEBUG} \
			-DAPP_VERSION_MAJOR=${VERSION_MAJOR} \
			-DAPP_VERSION_MINOR=${VERSION_MINOR} \
			-DAPP_VERSION_MICRO=${VERSION_MICRO}
BUILD_FLAGS_CXX := $(BUILD_FLAGS_CC) -fno-rtti -fno-exceptions -std=gnu++11
RUN_FLAGS :=

REMOTE_IP := 192.168.1.6

# 3DS/Wii U CONFIGURATION #

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    TITLE := $(NAME)
	ifeq ($(PKSV), 1)
		DESCRIPTION := Gen4+ pkmn save viewer
	else
		DESCRIPTION := Gen4+ pkmn save manager
	endif
    AUTHOR := Bernardo Giordano, PKSM devs
endif

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    LIBRARY_DIRS +=
    LIBRARIES +=

    PRODUCT_CODE := CTR-HB-PKSM
	ifeq ($(PKSV), 1)
		UNIQUE_ID := 0xEC200
	else
		UNIQUE_ID := 0xEC100
	endif

    CATEGORY := Application
    USE_ON_SD := true

    MEMORY_TYPE := Application
    SYSTEM_MODE := 64MB
    SYSTEM_MODE_EXT := Legacy
    CPU_SPEED := 268MHz
    ENABLE_L2_CACHE := true

    ICON_FLAGS := --flags visible,ratingrequired,recordusage --cero 153 --esrb 153 --usk 153 --pegigen 153 --pegiptr 153 --pegibbfc 153 --cob 153 --grb 153 --cgsrr 153

    ROMFS_DIR := assets/romfs
    BANNER_AUDIO := assets/audio.wav
	
	ifeq ($(PKSV), 1)
		BANNER_IMAGE := assets/banner_pksv.png
	else
		BANNER_IMAGE := assets/banner.png
	endif
	
	ifeq ($(PKSV), 1)
		ICON := assets/icon_pksv.png
	else
		ICON := assets/icon.png
	endif
	
	LOGO :=
endif

# INTERNAL #

include buildtools/make_base

pksm: 
	@make PKSM=1 PKSV=0 ROSALINA=0 DEBUG=0

citra:
	@make PKSM=1 PKSV=0 ROSALINA=0 DEBUG=1
	
pksv:
	@make PKSM=0 PKSV=1 ROSALINA=0 DEBUG=0
	
rosalinapksm:
	@make PKSM=0 PKSV=0 ROSALINA=1 DEBUG=0
	
rosalinapksv:
	@make PKSM=0 PKSV=1 ROSALINA=1 DEBUG=0

cleanall: 
	@rm -rf $(BUILD_BASE)
	@rm -rf $(OUTPUT_BASE)
	@echo Cleaned.