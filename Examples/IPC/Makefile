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

ipc_client.o: $(IPC_DIR)/ipc_client.c $(IPC_DIR)/ipc_client.h
	$(CC) -c $(IPC_DIR)/ipc_client.c $(CFLAGS) -I$(IPC_DIR)

ipc_recv: ipc_recv.c ipc_client.o $(IPC_DIR)/ipc_client.h $(IPC_DIR)/ipc_can.h
	$(CC) ipc_recv.c ipc_client.o $(CFLAGS) -I$(IPC_DIR) $(LDFLAGS) -o ipc_recv

ipc_send: ipc_send.c ipc_client.o $(IPC_DIR)/ipc_client.h $(IPC_DIR)/ipc_can.h
	$(CC) ipc_send.c ipc_client.o $(CFLAGS) -I$(IPC_DIR) $(LDFLAGS) -o ipc_send

clean:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2)

pristine:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2)
