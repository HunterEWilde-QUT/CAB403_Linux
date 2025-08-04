CC = gcc
CPPFLAGS = -I.
DEPS = hellomake.h
OBJ = hellomake.o hellofunc.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

hellomake: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)