MAKEFILES := fs-patch overlay
TARGETS		:= $(foreach dir,$(MAKEFILES),$(CURDIR)/$(dir))

# the below was taken from atmosphere + switch-examples makefile
export VERSION := 1.1.1
export GIT_BRANCH := release
export GIT_REVISION := $(VERSION)
export VERSION_DIRTY := $(VERSION)
export VERSION_WITH_HASH := $(VERSION)

export BUILD_DATE := -DDATE_YEAR=\"$(shell date +%Y)\" \
					-DDATE_MONTH=\"$(shell date +%m)\" \
					-DDATE_DAY=\"$(shell date +%d)\" \
					-DDATE_HOUR=\"$(shell date +%H)\" \
					-DDATE_MIN=\"$(shell date +%M)\" \
					-DDATE_SEC=\"$(shell date +%S)\" \

export CUSTOM_DEFINES := -DVERSION=\"v$(VERSION)\" \
					-DGIT_BRANCH=\"$(GIT_BRANCH)\" \
					-DGIT_REVISION=\"$(GIT_REVISION)\" \
					-DVERSION_DIRTY=\"$(VERSION_DIRTY)\" \
					-DVERSION_WITH_HASH=\"$(VERSION_WITH_HASH)\" \
					$(BUILD_DATE)

all: $(TARGETS)
	@mkdir -p out/
	@cp -R fs-patch/out/* out/
	@cp -R overlay/out/* out/

.PHONY: $(TARGETS)

$(TARGETS):
	@$(MAKE) -C $@

clean:
	@rm -rf out
	@for i in $(TARGETS); do $(MAKE) -C $$i clean || exit 1; done;

dist: all
	@for i in $(TARGETS); do $(MAKE) -C $$i dist || exit 1; done;
	@echo making dist ...

	@rm -f fs-patch.zip
	@cd out; zip -r ../fs-patch.zip ./*; cd ../
