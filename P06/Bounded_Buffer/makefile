CC = gcc
CPPFLAGS = -Wall
OBJ = testrc-dl.o

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

bbuff: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
