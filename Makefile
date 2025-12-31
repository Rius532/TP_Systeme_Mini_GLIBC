CC = gcc
# L'option -I./lib permet à GCC de trouver mini_lib.h sans changer le code source
CFLAGS = -Wall -I./lib

LIB_DIR = lib
CMD_DIR = cmds

# Liste des objets de la bibliothèque (avec le chemin)
# On compile lib/xxx.c pour faire xxx.o
LIB_OBJ = mini_memory.o mini_string.o mini_io.o

all: app mini_touch mini_cp

# --- Compilation des Exécutables ---

app: main.o $(LIB_OBJ)
	$(CC) $(CFLAGS) main.o $(LIB_OBJ) -o app.exe

mini_touch: mini_touch.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_touch.o $(LIB_OBJ) -o mini_touch

mini_cp: mini_cp.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_cp.o $(LIB_OBJ) -o mini_cp

# --- Règles de Compilation Génériques ---

# 1. Compilation des fichiers de la LIBRAIRIE + s COMMANDES / TESTS
# $< signifie "le fichier source", $@ signifie "le fichier objet"
mini_memory.o: $(LIB_DIR)/mini_memory.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(LIB_DIR)/mini_memory.c -o mini_memory.o

mini_string.o: $(LIB_DIR)/mini_string.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(LIB_DIR)/mini_string.c -o mini_string.o

mini_io.o: $(LIB_DIR)/mini_io.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(LIB_DIR)/mini_io.c -o mini_io.o

main.o: $(CMD_DIR)/main.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/main.c -o main.o

mini_touch.o: $(CMD_DIR)/mini_touch.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_touch.c -o mini_touch.o

mini_cp.o: $(CMD_DIR)/mini_cp.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_cp.c -o mini_cp.o

clean:
	rm -f *.exe mini_touch mini_cp *.o