#include "mini_lib.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        mini_printf("Usage: ");
        mini_printf(argv[0]);
        mini_printf(" <message>\n");
        mini_exit(1);
    }
    for(int i = 1; i < argc; i++) {
        mini_printf(argv[i]);
        if(i < argc - 1){
            mini_printf(" ");
        }
    }
    mini_printf("\n");
    mini_exit(0);
}