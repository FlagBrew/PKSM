#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#
# CONFIG_JSON is the filename of the NPDM config file (.json), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.json
#     - config.json
#   If a JSON file is provided or autodetected, an ExeFS PFS0 (.nsp) is built instead
#   of a homebrew executable (.nro). This is intended to be used for sysmodules.
#   NACP building is skipped as well.
#---------------------------------------------------------------------------------
$(call check_defined, PKSM_AUTHOR PKSM_DESCRIPTION PKSM_TITLE VERSION_MAJOR VERSION_MINOR VERSION_MICRO, Run make from the project directory)

APP_TITLE		:= 	$(PKSM_TITLE)
APP_DESCRIPTION	:=	$(PKSM_DESCRIPTION)
APP_AUTHOR		:=	$(PKSM_AUTHOR)

TARGET		:=	$(subst $e ,_,$(notdir $(APP_TITLE)))
OUT_DEBUG		:=	out
OUT_RELEASE		:=	out_release
BUILD_DEBUG		:=	build
BUILD_RELEASE	:=	build_release
ifneq ($(strip $(RELEASE)),)
BUILD			:=	$(BUILD_RELEASE)
OUTDIR			:=	$(OUT_RELEASE)
else
BUILD			:=	$(BUILD_DEBUG)
OUTDIR			:=	$(OUT_DEBUG)
endif
FORMATSOURCES	:= ../common/source/io \
					../common/source/picoc \
					../common/source/picoc/cstdlib \
					../common/source/sound \
					../common/source/sound/decoder \
					../common/source/utils \
					source
SOURCES			:=	$(FORMATSOURCES) \
					../common/source/quirc \
					../core/memecrypto \
					../core/source/i18n \
					../core/source/personal \
					../core/source/pkx \
					../core/source/sav \
					../core/source/utils \
					../core/source/wcx \
					../external/picoc/source/interpreter \
					../external/qrgen
DATA			:=	data
FORMATINCLUDES	:=	../common/include \
					../common/include/io \
					../common/include/sound \
					../common/include/sound/decoder \
					../common/include/utils \
					include \
					include/gui \
					include/gui/overlay 
INCLUDES		:=	$(FORMATINCLUDES) \
					../common/include/picoc \
					../common/include/quirc \
					../core/include \
					../core/include/enums \
					../core/include/personal \
					../core/include/pkx \
					../core/include/sav \
					../core/include/utils \
					../core/include/wcx \
					../core/memecrypto \
					../external \
					../external/fmt \
					../external/picoc/include \
					../external/qrgen
PACKER			:=	../external/EventsGalleryPacker
SCRIPTS			:=	../external/PKSM-Scripts
ROMFS			:=	../assets/romfs

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections -Wextra -Wno-psabi \
			-fomit-frame-pointer -ffunction-sections \
			-Wno-unused-parameter \
			-DQUIRC_MAX_REGIONS=10000 \
			-DUNIX_HOST \
			-DUNIQUE_ID=${UNIQUE_ID} \
			-DPKSM_PORT=34567 \
			-DFMT_HEADER_ONLY \
			-DJSON_HAS_FILESYSTEM=0 \
			-DJSON_HAS_EXPERIMENTAL_FILESYSTEM=0 \
			`curl-config --cflags` \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ -D_GNU_SOURCE=1

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++20 -fexceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

CXX		:=	`which ccache` $(CXX)
CC		:=	`which ccache` $(CC)

LIBS	:= -lnx \
		   `curl-config --libs` 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)/deps

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
export ROMFS_GFXFILES	:=	$(addprefix $(ROMFS)/gfx/,$(addsuffix .bz2, $(notdir $(T3XFILES) $(ROMFS_FONTFILES))))

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(ASSETWILDCARD),)
	export ASSETWILDCARD	:=	$(patsubst %, %/*, $(wildcard ../assets/gui_strings/*))
	export SCRIPTSDEPS		:=	$(shell find $(SCRIPTS)/src -type f -printf "\"%p\"\n" | sed -e 's/ /\\ /g' -e 's/"//g')
	export NEWGALLERY		:=	$(shell if [ -e $(PACKER)/EventsGallery ]; then cd $(PACKER)/EventsGallery && git rev-parse --short origin/master; else echo NOTHING; fi)
	export CURGALLERY		:=	$(shell if [ -e $(PACKER)/EventsGallery ]; then cd $(PACKER)/EventsGallery && git rev-parse --short HEAD; fi)
endif


ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: all binaries buildelf deps checkgallery directories clean spotless no-deps no-gifts no-scripts format cppcheck cppclean

#---------------------------------------------------------------------------------
# all: $(BUILD)

all:
	@$(MAKE) binaries _ROMFS_DEPS="$(ROMFS)/mg $(ROMFS)/scripts $(ROMFS)/i18n"
no-scripts:
	@$(MAKE) binaries _ROMFS_DEPS="$(ROMFS)/mg $(ROMFS)/i18n"
no-gifts:
	@$(MAKE) binaries _ROMFS_DEPS="$(ROMFS)/scripts $(ROMFS)/i18n"
no-deps:
	@$(MAKE) binaries _ROMFS_DEPS="$(ROMFS)/i18n"

#---------------------------------------------------------------------------------
deps: $(ROMFS)/mg $(ROMFS)/scripts $(ROMFS)/i18n | directories

$(ROMFS)/mg: $(PACKER)/EventsGallery
	@echo Packing events...
	@cd $(PACKER) && $(PYTHON) pack.py
	@mkdir -p $(ROMFS)/mg
	@cp $(PACKER)/out/*.bin.bz2 $(PACKER)/out/*.json.bz2 $(ROMFS)/mg
	@touch $(ROMFS)/mg

$(ROMFS)/scripts: $(SCRIPTSDEPS)
	@cd $(SCRIPTS) && $(PYTHON) genScripts.py
	@rm -fr $(ROMFS)/scripts
	@cd $(SCRIPTS) && mv -f scripts ../../assets/romfs

$(ROMFS)/i18n: $(filter-out %/gui.json, $(wildcard $(ASSETWILDCARD))) $(shell find ../core/strings -type f -printf "\"%p\"\n" | sed -e 's/ /\\ /g' -e 's/"//g')
	@echo Combining GUI strings...
	@cd .. && $(PYTHON) common/combine_strings_json.py
	@mkdir -p $(ROMFS)/i18n
	@rsync --recursive --include="gui.json" --filter="-! */" ../assets/gui_strings/* $(ROMFS)/i18n
	@cp -r ../core/strings/* $(ROMFS)/i18n
	@touch $(ROMFS)/i18n
$(PACKER)/EventsGallery: checkgallery

checkgallery:
ifneq ($(CURGALLERY),$(NEWGALLERY))
	@echo Updating EventsGallery
	@cd $(PACKER) && if [ -e EventsGallery ]; then echo Pulling EventsGallery...; cd EventsGallery; git pull -q; cd ..; touch EventsGallery; else echo Cloning EventsGallery...; git clone -q https://github.com/projectpokemon/EventsGallery.git; fi
endif

clean:
	@echo clean ...
	@rm -fr $(OUT_DEBUG)
	@rm -fr $(ROMFS)/mg/
	@rm -fr $(ROMFS)/i18n/
	@rm -fr $(ROMFS)/scripts
	@rm -fr $(BUILD_DEBUG) $(PACKER)/out $(PACKER)/EventsGallery

spotless: clean
	@rm -fr $(OUT_RELEASE)
	@rm -fr $(BUILD_RELEASE)
	
directories:
	@mkdir -p $(BUILD) $(OUTDIR) $(DEPSDIR)

$(OUTPUT).elf: $(ROMFS_GFXFILES) buildelf | directories
	@true

$(OUTPUT).nro: $(OUTPUT).elf
	@$(MAKE) -C $(BUILD) -f $(CURDIR)/Makefile $(OUTPUT).nro

binaries: $(OUTPUT).nro

#---------------------------------------------------------------------------------
format:
	clang-format -i -style=file $(foreach dir,$(FORMATSOURCES),$(wildcard $(dir)/*.c) $(wildcard $(dir)/*.cpp)) $(foreach dir,$(FORMATINCLUDES),$(wildcard $(dir)/*.h) $(wildcard $(dir)/*.hpp))
#---------------------------------------------------------------------------------
cppcheck:
	@rm -f $(BUILD)/cppcheck.log
	@mkdir -p $(BUILD)
	@cppcheck . ../core ../common --enable=all --std=c++20 -j $(shell nproc) $(INCLUDE) -UJSON_CATCH_USER -U_Check_return_ -U_MSC_VER -U_Ret_notnull_ -U__INTEL_COMPILER -U__PGI -U__SUNPRO_CC -UJSON_INTERNAL_CATCH_USER -UJSON_THROW_USER -UJSON_TRY_USER -U__IBMCPP__ -U__SUNPRO_CC -D__GNUC__=10 -D__GNUC_MINOR__=1 -DNULL=nullptr -D_PKSMCORE_EXTRA_LANGUAGES="NL, PT, RU, RO" --force 2> $(BUILD)/cppcheck.log
	@echo build/cppcheck.log file created...
#---------------------------------------------------------------------------------
cppclean:
	@rm -f $(BUILD)/cppclean.log
	@mkdir -p $(BUILD)
	@cppclean $(foreach dir,$(INCLUDES),--include-path=$(dir)) $(foreach dir,$(LIBDIRS),--include-path=$(dir)/include) --include-path=$(CURDIR)/$(BUILD) $(foreach dir,$(FORMATSOURCES),$(wildcard $(dir)/*.c) $(wildcard $(dir)/*.cpp)) $(foreach dir,$(FORMATINCLUDES),$(wildcard $(dir)/*.h) $(wildcard $(dir)/*.hpp)) > $(BUILD)/cppclean.log
	@echo build/cppclean.log created


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------