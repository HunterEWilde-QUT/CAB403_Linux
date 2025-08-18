CC = gcc
CPPFLAGS = -pthread -Wall
OBJ = msgq.o

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

pcthread: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
