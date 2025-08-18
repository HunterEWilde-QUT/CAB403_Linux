CC = gcc
CPPFLAGS = -pthread -Wall
OBJ = sampleThread.o

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

thread: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
