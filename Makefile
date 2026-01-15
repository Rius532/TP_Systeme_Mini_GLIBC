CC = gcc
CFLAGS = -Wall -I./lib

LIB_DIR = lib
CMD_DIR = cmds

LIB_SRC = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ = $(LIB_SRC:.c=.o)

COMMANDS = mini_touch mini_cp mini_echo mini_cat mini_head mini_tail \
           mini_clean mini_grep mini_wc mini_ls mini_chmod mini_ln \
           mini_quickdiff mini_mv mini_rm mini_rmdir mini_find mini_mkdir

all: app mini_shell $(COMMANDS)

# --- RÈGLES DE LIEN ---

app: main.o $(LIB_OBJ)
	$(CC) $(CFLAGS) main.o $(LIB_OBJ) -o main

mini_shell: mini_shell.o $(LIB_OBJ)
	$(CC) $(CFLAGS) mini_shell.o $(LIB_OBJ) -o mini_shell

$(COMMANDS): %: %.o $(LIB_OBJ)
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

# --- RÈGLES DE COMPILATION ---

$(LIB_DIR)/%.o: $(LIB_DIR)/%.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: $(CMD_DIR)/%.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c $(LIB_DIR)/mini_lib.h
	$(CC) $(CFLAGS) -c $< -o $@

# --- NETTOYAGE ---

clean:
	rm -f *.o $(LIB_DIR)/*.o

fclean: clean
	rm -f main app mini_shell $(COMMANDS)

rebuild: fclean all

.PHONY: all clean fclean rebuild