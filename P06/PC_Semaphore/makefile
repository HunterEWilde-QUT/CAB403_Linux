CC = gcc
CPPFLAGS = -Wall
DEPS = buffer.h
OBJ = skeleton_buffer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

pcbuff: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
