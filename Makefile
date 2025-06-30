export PKSM_TITLE		:= 	PKSM
export PKSM_DESCRIPTION	:=	Gen I to Gen VIII save manager
export PKSM_AUTHOR		:=	FlagBrew

export VERSION_MAJOR	:=	10
export VERSION_MINOR	:=	2
export VERSION_MICRO	:=	4
GIT_REV					:=	$(shell git rev-parse --short HEAD)
OLD_INFO				:=	$(shell if [ -e appinfo.hash ]; then cat appinfo.hash; fi)
NOW_INFO				:=	$(PKSM_TITLE) $(PKSM_DESCRIPTION) $(PKSM_AUTHOR) $(VERSION_MAJOR) $(VERSION_MINOR) $(VERSION_MICRO) $(GIT_REV)
REVISION_EXISTS			:=	$(shell if [ ! -e common/include/revision.h ]; then echo 1; fi)

OUTDIR			:= 	out
RELEASEDIR		:=	release
ICON			:=	assets/icon.png

ifeq ($(OS),Windows_NT)
 ifeq ($(shell where py),)
  export PYTHON := python
 else
  export PYTHON := py -3
 endif
else
 export PYTHON := python3
endif

debug: 3ds-debug switch-debug

release: 3ds-release docs switch

compile-commands: 3ds-compile-commands

no-deps: 3ds-no-deps
no-scripts: 3ds-no-scripts
no-gifts: 3ds-no-gifts

revision:
ifneq ($(NOW_INFO),$(OLD_INFO))
	@echo \#define GIT_REV \"$(GIT_REV)\" > common/include/revision.h
	@echo \#define VERSION_MAJOR $(VERSION_MAJOR) >> common/include/revision.h
	@echo \#define VERSION_MINOR $(VERSION_MINOR) >> common/include/revision.h
	@echo \#define VERSION_MICRO $(VERSION_MICRO) >> common/include/revision.h
	@echo "$(NOW_INFO)" > appinfo.hash
else
 ifneq ($(REVISION_EXISTS),)
	@echo \#define GIT_REV \"$(GIT_REV)\" > common/include/revision.h
	@echo \#define VERSION_MAJOR $(VERSION_MAJOR) >> common/include/revision.h
	@echo \#define VERSION_MINOR $(VERSION_MINOR) >> common/include/revision.h
	@echo \#define VERSION_MICRO $(VERSION_MICRO) >> common/include/revision.h
 endif
endif

3ds-debug: revision
	$(MAKE) -C 3ds

3ds-no-deps: revision
	$(MAKE) -C 3ds no-deps

3ds-no-scripts: revision
	$(MAKE) -C 3ds no-scripts

3ds-no-gifts: revision
	$(MAKE) -C 3ds no-gifts

3ds-release: revision
	$(MAKE) -C 3ds RELEASE="1"

3ds-compile-commands: revision
	$(MAKE) -C 3ds clean
	$(MAKE) -C 3ds GENERATE_COMPILE_COMMANDS=1
	@mv 3ds/build/compile_commands.json 3ds_compile_commands.json
	@echo 3DS compile commands written to 3ds_compile_commands.json

switch: revision
	$(MAKE) -C switch

switch-debug: revision
	$(MAKE) -C switch

docs:
	@mkdir -p $(OUTDIR)
	@gwtc -o $(OUTDIR) -n "$(PKSM_TITLE) Manual - v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO)" -t "$(APP_TITLE) v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO) Documentation" --logo-img $(ICON) docs/wiki
	@sed -i 's|\(<img[^>]*src="\)\(\./screenshots\)|\1https://raw.githubusercontent.com/wiki/FlagBrew/PKSM/screenshots|g' $(OUTDIR)/*.html

clean:
	@rm -f appinfo.hash
	@rm -f common/include/revision.h
	$(MAKE) -C 3ds clean
	$(MAKE) -C switch clean

clean-deps:
	@rm -f assets/gui_strings/*/gui.json
	$(MAKE) -C 3ds clean-deps

spotless: clean
	$(MAKE) -C 3ds spotless

format:
	$(MAKE) -C 3ds format
	$(MAKE) -C core format

cppcheck:
	$(MAKE) -C 3ds cppcheck

cppclean:
	$(MAKE) -C 3ds cppclean

.PHONY: debug release 3ds-debug switch-debug no-deps 3ds-release docs clean spotless format cppcheck cppclean
