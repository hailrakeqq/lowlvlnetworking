CC = clang

SRC_DIR = src
BUILD_DIR = build

SERVER_SRC = server.c
CLIENT_SRC = client.c

SERVER_BIN = echo_server
CLIENT_BIN = echo_client

all: $(BUILD_DIR) $(SERVER_BIN) $(CLIENT_BIN)
$(BUILD_DIR): 
	mkdir -p $(BUILD_DIR)

$(SERVER_BIN): $(SRC_DIR)/$(SERVER_SRC)
	$(CC) -o $(BUILD_DIR)/$@ $<

$(CLIENT_BIN): $(SRC_DIR)/$(CLIENT_SRC)
	$(CC) -o $(BUILD_DIR)/$@ $<
	
clean:
	rm -rf $(BUILD_DIR)

	