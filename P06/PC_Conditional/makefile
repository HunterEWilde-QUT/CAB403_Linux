CC = gcc
CPPFLAGS = -Wall
OBJ = ConditionVar_SKEL.o

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

pccond: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
