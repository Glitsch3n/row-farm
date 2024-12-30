BUILD_DIR := ./build
FILE_NAME = PipeBoy.ino

# emulator variables
ARDENS_FILE_PATH := /Applications/Ardens.app

# img conversion variables
CABI_FILE_PATH := /Applications/cabi
FILES = $(wildcard images/*.png)
OUTPUT_FILE = "TmpImages.h"


build:
	arduino-cli compile $(FILE_NAME) -b arduino:avr:leonardo --build-path $(BUILD_DIR)
	#$(ARDBITMAP_FILE_PATH)

.PHONY: launch
launch: build
	# readlink allow us to fetch the absolute file path of BUILD_DIR
	open $(ARDENS_FILE_PATH) --args -file=$(shell readlink -f $(BUILD_DIR))/$(FILE_NAME).hex

.PHONY: clean_imgs
clean_imgs:
	rm -f $(OUTPUT_FILE)

.PHONY: imgs
imgs: clean_imgs
	echo $(FILES)
	$(foreach file,\
		$(FILES),\
		$(CABI_FILE_PATH) $(file) $(basename $(notdir $(file))) >> $(OUTPUT_FILE);\
	)
	sed -i .bak '/const PROGMEM.*_mask\[/,/};/d; s/\/\/.*//' $(OUTPUT_FILE)
	rm $(OUTPUT_FILE).bak

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: re
re: clean build
