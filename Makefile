APP_TITLE		:= 	PKSM
APP_DESCRIPTION	:=	Gen IV to Gen VII save manager
APP_AUTHOR		:=	FlagBrew

VERSION_MAJOR	:=	6
VERSION_MINOR	:=	2
VERSION_MICRO	:=	0

OUTDIR			:= 	out
ICON			:=	assets/icon.png

all: 3ds docs

3ds:
	$(MAKE) -C 3ds

docs:
	@mkdir -p $(OUTDIR)
	@gwtc -o $(OUTDIR) -n "$(APP_TITLE) Manual" -t "$(APP_TITLE) v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO) Documentation" --logo-img $(ICON) docs/wiki

clean:
	$(MAKE) -C 3ds clean

.PHONY: 3ds docs clean
