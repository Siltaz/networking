# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Source files
SRCS = tcp_server.c tcp_client.c udp_server.c udp_client.c

# Build directory
BUILD_DIR = build

# Executables
TCP_SERVER = $(BUILD_DIR)/tcp_server
TCP_CLIENT = $(BUILD_DIR)/tcp_client
UDP_SERVER = $(BUILD_DIR)/udp_server
UDP_CLIENT = $(BUILD_DIR)/udp_client
CHAT_SERVER = $(BUILD_DIR)/chat_server
CHAT_CLIENT = $(BUILD_DIR)/chat_client

# Default target
all: $(TCP_SERVER) $(TCP_CLIENT) $(UDP_SERVER) $(UDP_CLIENT) $(CHAT_SERVER) $(CHAT_CLIENT)

# Create build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

# Build rules
$(TCP_SERVER): tcp_server.c
	$(CC) $(CFLAGS) -o $@ $<

$(TCP_CLIENT): tcp_client.c
	$(CC) $(CFLAGS) -o $@ $<

$(UDP_SERVER): udp_server.c
	$(CC) $(CFLAGS) -o $@ $<

$(UDP_CLIENT): udp_client.c
	$(CC) $(CFLAGS) -o $@ $<

$(CHAT_SERVER): chat_server.c
	$(CC) $(CFLAGS) -o $@ $<

$(CHAT_CLIENT): chat_client.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean rule
clean:
	rm -f $(TCP_SERVER) $(TCP_CLIENT) $(UDP_SERVER) $(UDP_CLIENT) $(CHAT_SERVER) $(CHAT_CLIENT)
