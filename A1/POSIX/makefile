CC = gcc
CPPFLAGS = -lrt -lpthread
DEPS = operations.h
OBJ = shm_ipc.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

posix: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
