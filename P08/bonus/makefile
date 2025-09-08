CC = gcc
CPPFLAGS = -Wall
OBJ = client.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

client: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
