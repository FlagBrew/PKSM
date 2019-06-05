#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

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
APP_TITLE		:= 	PKSM
APP_DESCRIPTION	:=	Gen IV to Gen VII save manager
APP_AUTHOR		:=	FlagBrew

TARGET			:=	$(subst $e ,_,$(notdir $(APP_TITLE)))
OUTDIR			:=	out
BUILD			:=	build
FORMATSOURCES	:=	../common/source \
					../common/source/io \
					../common/source/sound \
					../common/source/sound/decoder \
					../common/source/utils \
					../core/source \
					../core/source/i18n \
					../core/source/personal \
					../core/source/pkx \
					../core/source/sav \
					../core/source/wcx \
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
					../common/source/picoc \
					../common/source/picoc/cstdlib \
					../common/source/picoc/platform \
					../common/source/quirc \
					../core/memecrypto
DATA			:=	data
FORMATINCLUDES	:=	../common/include \
					../common/include/io \
					../common/include/sound \
					../common/include/sound/decoder \
					../common/include/utils \
					../core/include \
					../core/include/i18n \
					../core/include/personal \
					../core/include/pkx \
					../core/include/sav \
					../core/include/wcx \
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
					../core/memecrypto
GRAPHICS		:=	../assets/gfx/ui
ROMFS			:=	../assets/romfs
GFXBUILD		:=	$(ROMFS)/gfx
PACKER			:=	../common/EventsGalleryPacker
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

CFLAGS	:=	-g -Wall -Wextra -Wno-psabi -O2 -mword-relocations \
			-fomit-frame-pointer -ffunction-sections \
			-Wno-implicit-fallthrough -Wno-unused-parameter \
			$(ARCH) \
			-DQUIRC_MAX_REGIONS=10000 \
			-DUNIX_HOST \
			-DUNIQUE_ID=${UNIQUE_ID} \
			-DCITRA_DEBUG=${CITRA_DEBUG} \
			`sdl-config --cflags` \
			`arm-none-eabi-pkg-config opusfile --cflags`

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS -D_GNU_SOURCE=1

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lSDL_mixer -lSDL -lmpg123 -lvorbisidec -lopusfile -lopus -logg -lmikmod -lmad \
		   -lcurl -lmbedtls -lmbedx509 -lmbedcrypto -lbz2 \
		   -lz -lcitro2d -lcitro3d -lctru -lm

CXX		:= `which ccache` $(CXX)
CC		:= `which ccache` $(CC)

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

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
SHLISTFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.shlist)))
GFXFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.t3s)))
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

export OFILES_SOURCES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES)) \
			$(PICAFILES:.v.pica=.shbin.o) $(SHLISTFILES:.shlist=.shbin.o) \
			$(if $(filter $(BUILD),$(GFXBUILD)),$(addsuffix .o,$(T3XFILES)))

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES	:=	$(PICAFILES:.v.pica=_shbin.h) $(SHLISTFILES:.shlist=_shbin.h) \
			$(addsuffix .h,$(subst .,_,$(BINFILES))) \
			$(GFXFILES:.t3s=.h)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export _3DSXDEPS	:=	$(if $(NO_SMDH),,$(OUTPUT).smdh)

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

.PHONY: all clean deps no-deps format

#---------------------------------------------------------------------------------
all: deps no-deps
#---------------------------------------------------------------------------------
deps:
	@mkdir -p $(BUILD) $(GFXBUILD) $(OUTDIR)
ifeq ($(OS),Windows_NT)
	@cd $(PACKER) && py -3 packer.py
else
	@cd $(PACKER) && python3 packer.py
endif
	@cd $(PACKER) && mv out/*.bin.bz2 out/*.json.bz2 ../../assets/romfs/mg
ifeq ($(OS),Windows_NT)
	@cd $(SCRIPTS) && py -3 genScripts.py
else
	@cd $(SCRIPTS) && python3 genScripts.py
endif
	@rm -fr ../assets/romfs/scripts
	@cd $(SCRIPTS) && mv -f scripts ../../assets/romfs
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(OUTDIR)
	@cd $(ROMFS)/mg && find -maxdepth 1 ! -name .gitkeep ! -name . | xargs --no-run-if-empty rm
	@rm -fr $(BUILD) $(PACKER)/out $(PACKER)/EventsGallery
#---------------------------------------------------------------------------------
no-deps:
	@mkdir -p $(BUILD) $(GFXBUILD) $(OUTDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $(OUTPUT).3dsx
	@bannertool makebanner -i "$(BANNER_IMAGE)" -a "$(BANNER_AUDIO)" -o $(BUILD)/banner.bnr
	@bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -i "$(APP_ICON)" -f "$(ICON_FLAGS)" -o $(BUILD)/icon.icn
	@3dstool -ctf romfs "$(BUILD)/romfs.bin" --romfs-dir "$(ROMFS)"
	@makerom -f cia -o $(OUTPUT).cia -target t -exefslogo -elf "$(OUTPUT).elf" -rsf "$(RSF_PATH)" -ver "$$(($(VERSION_MAJOR)*1024+$(VERSION_MINOR)*16+$(VERSION_MICRO)))" -banner "$(BUILD)/banner.bnr" -icon "$(BUILD)/icon.icn" -DAPP_TITLE="$(APP_TITLE)" -DAPP_PRODUCT_CODE="$(PRODUCT_CODE)" -DAPP_UNIQUE_ID="$(UNIQUE_ID)" -romfs "$(BUILD)/romfs.bin"
#---------------------------------------------------------------------------------
format:
	clang-format -i -style=file $(foreach dir,$(FORMATSOURCES),$(wildcard $(dir)/*.c) $(wildcard $(dir)/*.cpp)) $(foreach dir,$(FORMATINCLUDES),$(wildcard $(dir)/*.h) $(wildcard $(dir)/*.hpp))
#---------------------------------------------------------------------------------
cppcheck:
	@rm -f $(BUILD)/cppcheck.log
	@cppcheck . --enable=all -I ../common 2> $(BUILD)/cppcheck.log
	@echo build/cppcheck.log file created...
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).3dsx	:	$(OUTPUT).elf $(_3DSXDEPS)

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
%.t3x.o	%_t3x.h :	%.t3x
#---------------------------------------------------------------------------------
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

#---------------------------------------------------------------------------------
%.t3x	%.h	:	%.t3s
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@tex3ds -i $< -H $*.h -d $*.d -o $(TOPDIR)/$(GFXBUILD)/$*.t3x

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
