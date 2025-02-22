OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')

ifeq ($(OS),Darwin)  # macOS
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = 
CC = clang
RM = rm -f
endif
ifeq ($(OS),Linux)  # Linux
CFLAGS = -Wall -Wextra -pthread
LDFLAGS =
CC = gcc
RM = rm -f
endif
IPC_DIR = ../../Sources/CANIPC

all: ipc_recv ipc_send

crc_j1850.o: $(IPC_DIR)/crc_j1850.c $(IPC_DIR)/crc_j1850.h
	$(CC) -c $(IPC_DIR)/crc_j1850.c $(CFLAGS) -I$(IPC_DIR)

tcp_client.o: $(IPC_DIR)/tcp_client.c $(IPC_DIR)/tcp_client.h
	$(CC) -c $(IPC_DIR)/tcp_client.c $(CFLAGS) -I$(IPC_DIR)

ipc_recv: ipc_recv.c tcp_client.o crc_j1850.o $(IPC_DIR)/tcp_client.h $(IPC_DIR)/tcp_can.h
	$(CC) ipc_recv.c tcp_client.o crc_j1850.o $(CFLAGS) -I$(IPC_DIR) $(LDFLAGS) -o ipc_recv

ipc_send: ipc_send.c tcp_client.o crc_j1850.o $(IPC_DIR)/tcp_client.h $(IPC_DIR)/tcp_can.h
	$(CC) ipc_send.c tcp_client.o crc_j1850.o $(CFLAGS) -I$(IPC_DIR) $(LDFLAGS) -o ipc_send

clean:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2)

pristine:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2)
