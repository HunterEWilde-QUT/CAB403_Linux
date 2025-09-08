CC = gcc
CPPFLAGS = -Wall
OBJ = Server_Ex1_Pseudocode.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

tcpserv: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
