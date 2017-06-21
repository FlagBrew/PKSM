# TARGET #

TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif
endif

# COMMON CONFIGURATION #

NAME := PKSV

BUILD_DIR := build/rosalina/PKSV
OUTPUT_DIR := output/rosalina/PKSV
INCLUDE_DIRS := 
SOURCE_DIRS := source/memecrypto/source source

EXTRA_OUTPUT_FILES :=

LIBRARY_DIRS := $(PORTLIBS) $(CTRULIB)
LIBRARIES := sfil sftd freetype png z sf2d citro3d ctru m

BUILD_FLAGS := -DPKSV -DROSALINA_3DSX -march=armv6k -mtune=mpcore -mfloat-abi=hard
BUILD_FLAGS_CC := -g -Wall -O2 -mword-relocations \
			-fomit-frame-pointer -ffast-math \
			$(BUILD_FLAGS) $(INCLUDE) -DARM11 -D_3DS
BUILD_FLAGS_CXX := $(BUILD_FLAGS_CC) -fno-rtti -fno-exceptions -std=gnu++11
RUN_FLAGS :=

VERSION_MAJOR := 2
VERSION_MINOR := 0
VERSION_MICRO := 0

REMOTE_IP := 192.168.1.7

# 3DS/Wii U CONFIGURATION #

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    TITLE := $(NAME)
    DESCRIPTION := Gen6+ Pokemon info viewer
    AUTHOR := Bernardo Giordano, PKSM devs
endif

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    LIBRARY_DIRS +=
    LIBRARIES +=

    PRODUCT_CODE := CTR-HB-PKSV
    UNIQUE_ID := 0xEC200

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
    BANNER_IMAGE := assets/banner_pksv.png
    ICON := assets/icon_pksv.png
	LOGO := 
endif

# INTERNAL #

include buildtools/make_base