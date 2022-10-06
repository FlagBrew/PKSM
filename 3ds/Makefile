#---------------------------------------------------------------------------------
.SUFFIXES:
.PRECIOUS: build/gfx/ui_sheet.t3x
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1$(if $2, ($2))))

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# GRAPHICS is a list of directories containing graphics files
# GFXBUILD is the directory where converted graphics files will be placed
#   If set to $(BUILD), it will statically link in the converted
#   files as if they were data files.
#
# NO_SMDH: if set to anything, no SMDH file is generated.
# ROMFS is the directory which contains the RomFS, relative to the Makefile (Optional)
# APP_TITLE is the name of the app stored in the SMDH file (Optional)
# APP_DESCRIPTION is the description of the app stored in the SMDH file (Optional)
# APP_AUTHOR is the author of the app stored in the SMDH file (Optional)
# ICON is the filename of the icon (.png), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.png
#     - icon.png
#     - <libctru folder>/default_icon.png
#---------------------------------------------------------------------------------

$(call check_defined, PKSM_AUTHOR PKSM_DESCRIPTION PKSM_TITLE VERSION_MAJOR VERSION_MINOR VERSION_MICRO, Run make from the project directory)

APP_TITLE		:= 	$(PKSM_TITLE)
APP_DESCRIPTION	:=	$(PKSM_DESCRIPTION)
APP_AUTHOR		:=	$(PKSM_AUTHOR)

TARGET			:=	$(subst $e ,_,$(notdir $(APP_TITLE)))
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
FORMATSOURCES	:=	../common/source \
					../common/source/gui \
					../common/source/io \
					../common/source/picoc \
					../common/source/picoc/cstdlib \
					../common/source/sound \
					../common/source/sound/decoder \
					../common/source/utils \
					source \
					source/gui \
					source/gui/overlay \
					source/gui/screen \
					source/gui/scripts \
					source/io \
					source/picoc \
					source/sound \
					source/titles \
					source/utils
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
					../common/include/gui \
					../common/include/io \
					../common/include/sound \
					../common/include/sound/decoder \
					../common/include/utils \
					include \
					include/gui \
					include/gui/overlay \
					include/gui/screen \
					include/gui/scripts \
					include/io \
					include/titles \
					include/utils
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

GRAPHICS		:=	../assets/gfx/ui
ROMFS			:=	../assets/romfs
GFXBUILD		:=	$(BUILD)/gfx
PACKER			:=	../external/EventsGalleryPacker
SCRIPTS			:=	../external/PKSM-Scripts
CITRA_DEBUG		:=	0

ICON			:=	../assets/icon.png
BANNER_AUDIO	:=	../assets/audio.wav
BANNER_IMAGE	:=	../assets/banner.png
RSF_PATH		:=	../assets/app.rsf
LOGO			:=
UNIQUE_ID		:=	0xEC100
PRODUCT_CODE	:=	CTR-HB-PKSM
ICON_FLAGS		:=	nosavebackups,visible

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

ifneq ($(strip $(RELEASE)),)
OPTIMIZE		:=	-O3 -flto
RELEASE_FLAGS	:=	-DNDEBUG
else
OPTIMIZE		:=	-Og
endif

CFLAGS	:=	-g -gdwarf-4 -Wall -Wextra -Wno-psabi -mword-relocations \
			-fomit-frame-pointer -ffunction-sections \
			-Wno-unused-parameter \
			$(ARCH) \
			-DQUIRC_MAX_REGIONS=10000 \
			-DUNIX_HOST \
			-DUNIQUE_ID=${UNIQUE_ID} \
			-DCITRA_DEBUG=${CITRA_DEBUG} \
			-DPKSM_PORT=34567 \
			-DFMT_HEADER_ONLY \
			-DJSON_HAS_FILESYSTEM=0 \
			-DJSON_HAS_EXPERIMENTAL_FILESYSTEM=0 \
			`arm-none-eabi-pkg-config libmpg123 --cflags` \
			`curl-config --cflags` \
			$(OPTIMIZE) $(RELEASE_FLAGS)

CFLAGS	+=	$(INCLUDE) -D__3DS__ -D_GNU_SOURCE=1

CXXFLAGS	:=	$(CFLAGS) -fno-rtti -std=gnu++20

ASFLAGS	:=	-g -gdwarf-4 $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g -gdwarf-4 $(ARCH) -Wl,-Map,$(notdir $*.map) -Wl,--wrap=abort $(OPTIMIZE)

LIBS	:=	-lcitro2d -lcitro3d -lbz2 \
			`arm-none-eabi-pkg-config libmpg123 --libs` \
			`curl-config --libs`

CXX		:=	`which ccache` $(CXX)
CC		:=	`which ccache` $(CC)

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CTRULIB) $(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)/deps

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
SHLISTFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.shlist)))
GFXFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.t3s)))
FONTFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.ttf)))
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

export T3XFILES		:=	$(GFXFILES:.t3s=.t3x)
export ROMFS_FONTFILES	:=	$(patsubst %.ttf, $(GFXBUILD)/%.bcfnt, $(FONTFILES))

export ROMFS_GFXFILES	:=	$(addprefix $(ROMFS)/gfx/,$(addsuffix .bz2, $(notdir $(T3XFILES) $(ROMFS_FONTFILES))))

export OFILES_SOURCES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES)) \
			$(PICAFILES:.v.pica=.shbin.o) $(SHLISTFILES:.shlist=.shbin.o) \
			# $(if $(filter $(BUILD),$(GFXBUILD)),$(addsuffix .o,$(T3XFILES)))

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES	:=	$(PICAFILES:.v.pica=_shbin.h) $(SHLISTFILES:.shlist=_shbin.h) \
			$(addsuffix .h,$(subst .,_,$(BINFILES))) \
			$(GFXFILES:.t3s=.h)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export _3DSXDEPS	:=	$(if $(NO_SMDH),,$(OUTPUT).smdh)

ifeq ($(ASSETWILDCARD),)
	export ASSETWILDCARD	:=	$(patsubst %, %/*, $(wildcard ../assets/gui_strings/*))
	export SCRIPTSDEPS		:=	$(shell find $(SCRIPTS)/src -type f -printf "\"%p\"\n" | sed -e 's/ /\\ /g' -e 's/"//g')
	export NEWGALLERY		:=	$(shell if [ -e $(PACKER)/EventsGallery ]; then cd $(PACKER)/EventsGallery && git rev-parse --short origin/master; else echo NOTHING; fi)
	export CURGALLERY		:=	$(shell if [ -e $(PACKER)/EventsGallery ]; then cd $(PACKER)/EventsGallery && git rev-parse --short HEAD; fi)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(CURDIR)/$(OUTDIR)/$(TARGET).smdh
endif

ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)
endif

.PHONY: all binaries buildelf deps checkgallery directories clean spotless no-deps no-gifts no-scripts format cppcheck cppclean

#---------------------------------------------------------------------------------
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
	@cp $(PACKER)/out/*.bin.bz2 $(PACKER)/out/*.json.bz2 $(ROMFS)/mg
	@touch $(ROMFS)/mg

$(ROMFS)/scripts: $(SCRIPTSDEPS)
	@cd $(SCRIPTS) && $(PYTHON) genScripts.py
	@rm -fr $(ROMFS)/scripts
	@cd $(SCRIPTS) && mv -f scripts ../../assets/romfs

$(ROMFS)/i18n: $(filter-out %/gui.json, $(wildcard $(ASSETWILDCARD)))
	@echo Combining GUI strings...
	@cd .. && $(PYTHON) common/combine_strings_json.py
	@mkdir -p $(ROMFS)/i18n
	@rsync --recursive --include="gui.json" --filter="-! */" ../assets/gui_strings/* $(ROMFS)/i18n
	@touch $(ROMFS)/i18n

$(PACKER)/EventsGallery: checkgallery

checkgallery:
ifneq ($(CURGALLERY),$(NEWGALLERY))
	@cd $(PACKER) && if [ -e EventsGallery ]; then cd EventsGallery && git pull > /dev/null && cd .. && touch EventsGallery; else git clone https://github.com/projectpokemon/EventsGallery.git > /dev/null; fi
endif
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(OUT_DEBUG)
	@rm -fr $(ROMFS)/mg/
	@rm -fr $(ROMFS)/i18n/
	@rm -fr $(ROMFS)/scripts
	@touch $(ROMFS)/mg/.gitkeep
	@rm -fr $(BUILD_DEBUG) $(PACKER)/out $(PACKER)/EventsGallery
	@rm -fr $(GFXBUILD)
#---------------------------------------------------------------------------------
spotless: clean
	@rm -fr $(OUT_RELEASE)
	@rm -fr $(BUILD_RELEASE)
#---------------------------------------------------------------------------------
directories:
	@mkdir -p $(BUILD) $(GFXBUILD) $(OUTDIR) $(ROMFS)/gfx $(DEPSDIR)
#---------------------------------------------------------------------------------
$(OUTPUT).3dsx: $(OUTPUT).elf $(ROMFS_GFXFILES) $(_ROMFS_DEPS) ../appinfo.hash | directories
	@$(MAKE) -C $(BUILD) -f $(CURDIR)/Makefile $(OUTPUT).3dsx

$(OUTPUT).cia: $(OUTPUT).elf $(BUILD)/banner.bnr $(BUILD)/icon.icn $(BUILD)/romfs.bin ../appinfo.hash
	@makerom -f cia -o $(OUTPUT).cia -target t -exefslogo -elf "$(OUTPUT).elf" -rsf "$(RSF_PATH)" -ver "$$(($(VERSION_MAJOR)*1024+$(VERSION_MINOR)*16+$(VERSION_MICRO)))" -banner "$(BUILD)/banner.bnr" -icon "$(BUILD)/icon.icn" -DAPP_TITLE="$(APP_TITLE)" -DAPP_PRODUCT_CODE="$(PRODUCT_CODE)" -DAPP_UNIQUE_ID="$(UNIQUE_ID)" -romfs "$(BUILD)/romfs.bin"

$(BUILD)/banner.bnr: $(BANNER_IMAGE) $(BANNER_AUDIO)
	@bannertool makebanner -i "$(BANNER_IMAGE)" -a "$(BANNER_AUDIO)" -o $(BUILD)/banner.bnr

$(BUILD)/icon.icn: $(APP_ICON) ../appinfo.hash
	@bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -i "$(APP_ICON)" -f "$(ICON_FLAGS)" -o $(BUILD)/icon.icn

$(BUILD)/romfs.bin: $(ROMFS_GFXFILES) $(_ROMFS_DEPS) | directories
	@3dstool -ctf romfs "$(BUILD)/romfs.bin" --romfs-dir "$(ROMFS)"

$(OUTPUT).elf: buildelf
	@true

buildelf:
	@$(MAKE) -C $(BUILD) -f $(CURDIR)/Makefile $(OUTPUT).elf

../appinfo.hash:
	@$(error appinfo.hash not found. Please run make from the project directory)

binaries: $(OUTPUT).3dsx $(OUTPUT).cia

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
$(GFXBUILD)/%.bcfnt :		%.ttf | directories
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@mkbcfnt -o $(GFXBUILD)/$*.bcfnt $<

#---------------------------------------------------------------------------------
$(GFXBUILD)/%.t3x	$(BUILD)/%.h	:	%.t3s | directories
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@tex3ds -i $< -H $(BUILD)/$*.h -d $(DEPSDIR)/$*.d -o $@

#---------------------------------------------------------------------------------
$(ROMFS)/gfx/%.bz2 : $(GFXBUILD)/% | directories
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@bzip2 -kf --best $< -c > $@

else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).3dsx	:	$(OUTPUT).elf $(_3DSXDEPS) $(ROMFS_GFXFILES:%=../%) $(_ROMFS_DEPS:%=../%) ../../appinfo.hash

$(OFILES_SOURCES) : $(HFILES)

$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# rules for assembling GPU shaders
#---------------------------------------------------------------------------------
define shader-as
	$(eval CURBIN := $*.shbin)
	$(eval DEPSFILE := $(DEPSDIR)/$*.shbin.d)
	echo "$(CURBIN).o: $< $1" > $(DEPSFILE)
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u32" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(CURBIN) | tr . _)`.h
	picasso -o $(CURBIN) $1
	bin2s $(CURBIN) | $(AS) -o $*.shbin.o
endef

%.shbin.o %_shbin.h : %.v.pica %.g.pica
	@echo $(notdir $^)
	@$(call shader-as,$^)

%.shbin.o %_shbin.h : %.v.pica
	@echo $(notdir $<)
	@$(call shader-as,$<)

%.shbin.o %_shbin.h : %.shlist
	@echo $(notdir $<)
	@$(call shader-as,$(foreach file,$(shell cat $<),$(dir $<)$(file)))

overrides.o:CXXFLAGS+=-fno-lto

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
