CC = gcc
CPPFLAGS = -Wall
OBJ = UDP_Server_Pseudocode.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

udpserv: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
