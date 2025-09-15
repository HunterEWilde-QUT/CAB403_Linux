CC = gcc
CPPFLAGS = -Wall
OBJ = roundRobinScheduling_Skeleton.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

rrscheduling: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)
