CC = clang

SRC_DIR = src
BUILD_DIR = build

SOURCE_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/handleClient.c
OUTPUT_BIN = httpServer

all: $(BUILD_DIR) $(OUTPUT_BIN)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OUTPUT_BIN):
	$(CC) -o $(BUILD_DIR)/$@ $(SOURCE_FILES)

clean:
	rm -rf $(BUILD_DIR)
