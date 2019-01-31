APP_TITLE		:= 	PKSM
APP_DESCRIPTION	:=	Gen IV to Gen VII save manager
APP_AUTHOR		:=	FlagBrew

VERSION_MAJOR	:=	6
VERSION_MINOR	:=	2
VERSION_MICRO	:=	0

OUTDIR			:= 	out
ICON			:=	assets/icon.png

<<<<<<< HEAD
all: 3ds docs
=======
BANNER_AUDIO	:=	assets/audio.wav
BANNER_IMAGE	:=	assets/banner.png
RSF_PATH		:=	assets/app.rsf


# If left blank, makerom will use the default Homebrew logo
LOGO			:=

# If left blank, makerom will use default values (0xff3ff and CTR-P-CTAP, respectively)
UNIQUE_ID		:=	0xEC100
PRODUCT_CODE	:=	CTR-HB-PKSM

# Don't really need to change this
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
			-DVERSION_MAJOR=${VERSION_MAJOR} \
			-DVERSION_MINOR=${VERSION_MINOR} \
			-DVERSION_MICRO=${VERSION_MICRO} \
			-DUNIQUE_ID=${UNIQUE_ID} \
			-DGIT_REV=\"$(GIT_REV)\" \
			-DCITRA_DEBUG=${CITRA_DEBUG} \
			`sdl-config --cflags` \
			-DPICOC_DEBUG

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS -D_GNU_SOURCE=1

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lSDL_mixer -lSDL -lmpg123 -lvorbisidec -logg -lmikmod -lmad  \
		   -lcurl -lmbedtls -lmbedx509 -lmbedcrypto \
		   -lz -lcitro2d -lcitro3d -lctru -lm 

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
>>>>>>> Multifont support

3ds:
	$(MAKE) -C 3ds

docs:
	@mkdir -p $(OUTDIR)
	@gwtc -o $(OUTDIR) -n "$(APP_TITLE) Manual" -t "$(APP_TITLE) v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO) Documentation" --logo-img $(ICON) docs/wiki

clean:
	$(MAKE) -C 3ds clean

.PHONY: 3ds docs clean
