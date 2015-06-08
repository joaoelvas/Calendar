CC = gcc
CFLAGS = -g -Wall -I sqlite3
LDFLAGS = -lpthread -ldl
DEPS = mysocks.h calendar.h client.h event.h
OBJ_S = calendar.o mysocks.o server.o event.o
OBJ_C = client.o mysocks.o event.o client_console.o client_communicator.o
SQLITE3 = sqlite3/sqlite3.o

all: server client

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ_S) $(SQLITE3)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)
	
client: $(OBJ_C)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)	
	
clean:
	rm -rf 	$(OBJ_S) $(OBJ_C) server client