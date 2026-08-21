# Host tests. No devkitPro, no libctru: these cover the parts of PKSM that are pure.
#   make -C tests

CXX      ?= g++
CXXFLAGS := -std=gnu++20 -g -Wall -Wextra -Werror -Og
INCLUDE  := -I../common/include -I../common/include/utils -I../core/include -I../external

BUILD   := build
TARGETS := $(BUILD)/BankFileTests $(BUILD)/GpssPageTests $(BUILD)/PresenterTests \
	$(BUILD)/ScreenStackTests

.PHONY: all clean
all: $(TARGETS)
	@for test in $(TARGETS); do ./$$test || exit 1; done

$(BUILD)/BankFileTests: BankFileTests.cpp ../common/source/BankFile.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/GpssPageTests: GpssPageTests.cpp ../common/source/GpssPage.cpp \
		../common/source/utils/base64.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/PresenterTests: PresenterTests.cpp ../common/source/Presenter.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^ -pthread

$(BUILD)/ScreenStackTests: ScreenStackTests.cpp ../common/source/gui/ScreenStack.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -Ifakes -I../common/include/gui -o $@ $^

$(BUILD):
	@mkdir -p $(BUILD)

clean:
	@rm -rf $(BUILD)
