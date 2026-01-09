CC = gcc
# L'option -I./lib permet à GCC de trouver mini_lib.h sans changer le code source
CFLAGS = -Wall -I./lib

LIB_DIR = lib
CMD_DIR = cmds

# Liste des objets de la bibliothèque (avec le chemin)
# On compile lib/xxx.c pour faire xxx.o
LIB_OBJ = mini_memory.o mini_string.o mini_io.o

all: app mini_touch mini_cp mini_echo mini_cat mini_head mini_tail mini_clean mini_grep mini_wc mini_ls mini_chmod mini_ln

# --- Compilation des Exécutables ---

app: main.o $(LIB_OBJ)
	$(CC) $(CFLAGS) main.o $(LIB_OBJ) -o app.exe

mini_touch: mini_touch.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_touch.o $(LIB_OBJ) -o mini_touch

mini_cp: mini_cp.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_cp.o $(LIB_OBJ) -o mini_cp

mini_echo: mini_echo.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_echo.o $(LIB_OBJ) -o mini_echo

mini_cat: mini_cat.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_cat.o $(LIB_OBJ) -o mini_cat

mini_head: mini_head.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_head.o $(LIB_OBJ) -o mini_head

mini_tail: mini_tail.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_tail.o $(LIB_OBJ) -o mini_tail

mini_clean: mini_clean.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_clean.o $(LIB_OBJ) -o mini_clean

mini_grep: mini_grep.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_grep.o $(LIB_OBJ) -o mini_grep

mini_wc: mini_wc.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_wc.o $(LIB_OBJ) -o mini_wc

mini_ls: mini_ls.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_ls.o $(LIB_OBJ) -o mini_ls

mini_chmod: mini_chmod.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_chmod.o $(LIB_OBJ) -o mini_chmod

mini_ln: mini_ln.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_ln.o $(LIB_OBJ) -o mini_ln

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

mini_echo.o: $(CMD_DIR)/mini_echo.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_echo.c -o mini_echo.o

mini_cat.o: $(CMD_DIR)/mini_cat.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_cat.c -o mini_cat.o

mini_head.o: $(CMD_DIR)/mini_head.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_head.c -o mini_head.o

mini_tail.o: $(CMD_DIR)/mini_tail.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_tail.c -o mini_tail.o

mini_clean.o: $(CMD_DIR)/mini_clean.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_clean.c -o mini_clean.o

mini_grep.o: $(CMD_DIR)/mini_grep.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_grep.c -o mini_grep.o

mini_wc.o: $(CMD_DIR)/mini_wc.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_wc.c -o mini_wc.o

mini_ls.o: $(CMD_DIR)/mini_ls.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_ls.c -o mini_ls.o

mini_chmod.o: $(CMD_DIR)/mini_chmod.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_chmod.c -o mini_chmod.o

mini_ln.o: $(CMD_DIR)/mini_ln.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $(CMD_DIR)/mini_ln.c -o mini_ln.o

# 1. Nettoyage standard : supprime seulement les objets (.o)
clean:
	rm -f *.o

# 2. Nettoyage complet : appelle 'clean' PUIS supprime les exécutables
fclean: clean
	rm -f app.exe mini_touch mini_cp mini_cat mini_echo mini_cat mini_head mini_tail mini_clean mini_grep mini_wc mini_ls mini_chmod mini_ln

rebuild: fclean all