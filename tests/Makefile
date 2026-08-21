# Host tests. No devkitPro, no libctru: these cover the parts of PKSM that are pure.
#   make -C tests

CXX      ?= g++
CXXFLAGS := -std=gnu++20 -g -Wall -Wextra -Werror -Og
INCLUDE  := -I../common/include -I../core/include

BUILD   := build
TARGETS := $(BUILD)/BankFileTests

.PHONY: all clean
all: $(TARGETS)
	@for test in $(TARGETS); do ./$$test || exit 1; done

$(BUILD)/BankFileTests: BankFileTests.cpp ../common/source/BankFile.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD):
	@mkdir -p $(BUILD)

clean:
	@rm -rf $(BUILD)
