CC = gcc
CFLAGS = -Wall
LIB_OBJ = mini_memory.o mini_string.o mini_io.o
all: app mini_touch
app: main.o $(LIB_OBJ)
	$(CC) $(CFLAGS) main.o $(LIB_OBJ) -o app.exe

mini_touch: mini_touch.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_touch.o $(LIB_OBJ) -o mini_touch

main.o: main.c mini_lib.h
mini_touch.o: mini_touch.c mini_lib.h
mini_memory.o: mini_memory.c mini_lib.h
mini_string.o: mini_string.c mini_lib.h
mini_io.o: mini_io.c mini_lib.h

clean:
	rm -f app.exe mini_touch *.o