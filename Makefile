APP_TITLE		:= 	PKSM
APP_DESCRIPTION	:=	Gen IV to Gen VII save manager
APP_AUTHOR		:=	FlagBrew

VERSION_MAJOR	:=	9
VERSION_MINOR	:=	0
VERSION_MICRO	:=	1
GIT_REV="$(shell git rev-parse --short HEAD)"

OUTDIR			:= 	out
RELEASEDIR		:=	release
ICON			:=	assets/icon.png

debug: 3ds-debug

release: 3ds-release docs

revision:
	@echo \#define GIT_REV \"$(GIT_REV)\" > common/include/revision.h
	@echo \#define VERSION_MAJOR $(VERSION_MAJOR) >> common/include/revision.h
	@echo \#define VERSION_MINOR $(VERSION_MINOR) >> common/include/revision.h
	@echo \#define VERSION_MICRO $(VERSION_MICRO) >> common/include/revision.h

3ds-debug: revision
	$(MAKE) -C 3ds VERSION_MAJOR=$(VERSION_MAJOR) VERSION_MINOR=$(VERSION_MINOR) VERSION_MICRO=$(VERSION_MICRO)

no-deps: revision
	$(MAKE) -C 3ds VERSION_MAJOR=$(VERSION_MAJOR) VERSION_MINOR=$(VERSION_MINOR) VERSION_MICRO=$(VERSION_MICRO) no-deps

3ds-release: revision
	$(MAKE) -C 3ds VERSION_MAJOR=$(VERSION_MAJOR) VERSION_MINOR=$(VERSION_MINOR) VERSION_MICRO=$(VERSION_MICRO) RELEASE="1"

docs:
	@mkdir -p $(OUTDIR)
	@gwtc -o $(OUTDIR) -n "$(APP_TITLE) Manual" -t "$(APP_TITLE) v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO) Documentation" --logo-img $(ICON) docs/wiki

clean:
	@rm -f common/include/revision.h
	$(MAKE) -C 3ds clean

spotless: clean
	$(MAKE) -C 3ds spotless

format:
	$(MAKE) -C 3ds format
	$(MAKE) -C core format

cppcheck:
	$(MAKE) -C 3ds cppcheck

cppclean:
	$(MAKE) -C 3ds cppclean

.PHONY: debug release revision 3ds-debug no-deps 3ds-release docs clean spotless format cppcheck cppclean
