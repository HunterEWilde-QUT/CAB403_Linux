CC = gcc
CPPFLAGS = -I.
CFLAGS = -Wall
DEPS = internal.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)
call: call.o
	$(CC) -o $@ $^ $(CPPFLAGS)
car: car.o
	$(CC) -o $@ $^ $(CPPFLAGS)
controller: controller.o
	$(CC) -o $@ $^ $(CPPFLAGS)
internal: internal.o
	$(CC) -o $@ $^ $(CPPFLAGS)
safety: safety.o
	$(CC) -o $@ $^ $(CPPFLAGS)