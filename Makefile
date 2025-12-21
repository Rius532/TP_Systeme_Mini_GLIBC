CC = gcc
CFLAGS = -Wall
OBJ = main.o mini_memory.o mini_string.o

app: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o app.exe

main.o: main.c 
mini_memory.o:  mini_memory.c mini_lib.h

clean:
	rm -f app $(OBJ)

