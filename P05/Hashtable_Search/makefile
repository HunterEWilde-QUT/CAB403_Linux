CC = gcc
CPPFLAGS = -Wall
OBJ = hashtable_search.o

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

htsearch: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
