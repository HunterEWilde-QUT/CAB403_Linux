CC = gcc
DEPS = dbl_vector.h
OBJ = dbl_vector.o test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

vector: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
