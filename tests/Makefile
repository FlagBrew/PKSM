# Host tests. No devkitPro, no libctru: these cover the parts of PKSM that are pure.
#   make -C tests

CXX      ?= g++
CXXFLAGS := -std=gnu++20 -g -Wall -Wextra -Werror -Og
INCLUDE  := -I../common/include -I../common/include/gui -I../common/include/io \
	-I../common/include/utils -I../core/include -I../external -I../external/qrgen \
	-I../3ds/include

BUILD   := build

TARGETS := $(BUILD)/BankFileTests $(BUILD)/CameraFrameTests \
	$(BUILD)/ConfigurationFileTests $(BUILD)/DirectoryBrowserTests \
	$(BUILD)/GpssPageTests $(BUILD)/PresenterTests \
	$(BUILD)/SaveIndexTests $(BUILD)/SaveRecognitionTests $(BUILD)/ScreenStackTests \
	$(BUILD)/SubsystemsTests \
  $(BUILD)/TransferProtocolTests

QUIRC_OBJECTS := $(BUILD)/quirc.o $(BUILD)/quirc_decode.o $(BUILD)/quirc_identify.o \
	$(BUILD)/quirc_version_db.o

.PHONY: all clean
all: $(TARGETS)
	@for test in $(TARGETS); do ./$$test || exit 1; done

$(BUILD)/BankFileTests: BankFileTests.cpp ../common/source/BankFile.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/CameraFrameTests: CameraFrameTests.cpp ../external/qrgen/QRGen.cpp \
		../common/include/gui/CameraFrame.hpp $(QUIRC_OBJECTS) | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $(filter %.cpp %.o,$^)

$(BUILD)/quirc.o: ../common/source/quirc/quirc.c | $(BUILD)
	@$(CC) -std=gnu11 -g -Wall -Wextra -Werror -Og -DQUIRC_MAX_REGIONS=10000 \
		-I../common/include/quirc -c -o $@ $<

$(BUILD)/quirc_%.o: ../common/source/quirc/%.c | $(BUILD)
	@$(CC) -std=gnu11 -g -Wall -Wextra -Werror -Og -DQUIRC_MAX_REGIONS=10000 \
		-I../common/include/quirc -c -o $@ $<

$(BUILD)/ConfigurationFileTests: ConfigurationFileTests.cpp \
		../common/source/ConfigurationFile.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/DirectoryBrowserTests: DirectoryBrowserTests.cpp \
		../common/source/io/DirectoryBrowser.cpp ../common/source/io/STDirectory.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/GpssPageTests: GpssPageTests.cpp ../common/source/GpssPage.cpp \
		../common/source/utils/base64.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/PresenterTests: PresenterTests.cpp ../common/source/Presenter.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^ -pthread

$(BUILD)/SaveIndexTests: SaveIndexTests.cpp ../common/source/SaveIndex.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/SaveRecognitionTests: SaveRecognitionTests.cpp \
		../core/source/sav/SaveRecognition.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/ScreenStackTests: ScreenStackTests.cpp ../common/source/gui/ScreenStack.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -Ifakes -I../common/include/gui -o $@ $^

$(BUILD)/SubsystemsTests: SubsystemsTests.cpp ../common/source/Subsystems.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD)/TransferProtocolTests: TransferProtocolTests.cpp \
		../common/source/utils/TransferProtocol.cpp | $(BUILD)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

$(BUILD):
	@mkdir -p $(BUILD)

clean:
	@rm -rf $(BUILD)
