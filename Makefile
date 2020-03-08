APP_TITLE		:= 	PKSM
APP_DESCRIPTION	:=	Gen IV to Gen VII save manager
APP_AUTHOR		:=	FlagBrew

VERSION_MAJOR	:=	8
VERSION_MINOR	:=	0
VERSION_MICRO	:=	3
GIT_REV="$(shell git rev-parse --short HEAD)"

OUTDIR			:= 	out
RELEASEDIR		:=	release
ICON			:=	assets/icon.png

all: 3ds

release: 3ds docs
	@mkdir -p $(RELEASEDIR)
	@cp 3ds/out/PKSM.elf $(RELEASEDIR)

revision:
	@echo \#define GIT_REV \"$(GIT_REV)\" > common/include/revision.h
	@echo \#define VERSION_MAJOR $(VERSION_MAJOR) >> common/include/revision.h
	@echo \#define VERSION_MINOR $(VERSION_MINOR) >> common/include/revision.h
	@echo \#define VERSION_MICRO $(VERSION_MICRO) >> common/include/revision.h

3ds: revision
	$(MAKE) -C 3ds VERSION_MAJOR=$(VERSION_MAJOR) VERSION_MINOR=$(VERSION_MINOR) VERSION_MICRO=$(VERSION_MICRO)

no-deps: revision
	$(MAKE) -C 3ds VERSION_MAJOR=$(VERSION_MAJOR) VERSION_MINOR=$(VERSION_MINOR) VERSION_MICRO=$(VERSION_MICRO) no-deps

docs:
	@mkdir -p $(OUTDIR)
	@gwtc -o $(OUTDIR) -n "$(APP_TITLE) Manual" -t "$(APP_TITLE) v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO) Documentation" --logo-img $(ICON) docs/wiki

clean:
	@rm -f common/include/revision.h
	$(MAKE) -C 3ds clean

format:
	$(MAKE) -C 3ds format
	$(MAKE) -C core format

cppcheck:
	$(MAKE) -C 3ds cppcheck

cppclean:
	$(MAKE) -C 3ds cppclean

.PHONY: revision 3ds docs clean format cppcheck release
