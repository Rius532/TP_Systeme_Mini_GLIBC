#include "mini_lib.h"

#define DEBUG

#ifdef DEBUG
    #include <stdio.h>
    #define LOG_TEST(fmt, ...) printf("[TEST] " fmt "\n", ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...) printf("       -> " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_TEST(fmt, ...)
    #define LOG_INFO(fmt, ...)
#endif


void test_basic_allocation() {
    mini_printf("\n=== TEST 1: Allocation & Libération Simple ===\n");
    int initial_used = nb_ptr_utilisee();

    void* p1 = mini_calloc(10, 10);
    LOG_INFO("Pointeur p1 alloué: %p", p1);
    LOG_INFO("Nombre elements utilises: %d", nb_ptr_utilisee());

    mini_free(p1);
    LOG_INFO("P1 libéré");
    LOG_INFO("Nombre elements utilises: %d", nb_ptr_utilisee());
    
    if (nb_ptr_utilisee() == initial_used) {
        mini_printf("[OK] La memoire est vide.\n");
    } else {
        mini_printf("[FAIL] Fuite de memoire detectee.\n");
    }
}

void test_reusable_memory() {
    mini_printf("\n=== TEST 2: Réutilisation de Mémoire ===\n");
    
    void* p_big = mini_calloc(1, 100); 
    LOG_INFO("Allocation Grand Bloc (100 octets): %p", p_big);
    
    mini_free(p_big);
    LOG_INFO("Liberation Grand Bloc");

    void* p_small = mini_calloc(1, 20);
    LOG_INFO("Allocation Petit Bloc (20 octets): %p", p_small);

    if (p_big == p_small) {
        mini_printf("[OK] SUCCESS: Le pointeur a ete reutilise !\n");
    } else {
        mini_printf("[FAIL] WARNING: Nouvelle allocation au lieu de reutilisation.\n");
        LOG_INFO("Diff: p_big=%p vs p_small=%p", p_big, p_small);
    }
    
    mini_free(p_small);
}

void test_mini_string() {
    mini_printf("\n=== TEST 3: Mini String & Buffer ===\n");
    mini_printf("Cette phrase est envoyee a mini_printf.\n");
    mini_printf("Celle-ci aussi, sans saut de ligne...");
    mini_printf(" Et la suite ici.\n");
}

void test_mini_scanf() {
    mini_printf("\n=== TEST 4: Mini Scanf & Buffer Overflow Protection ===\n");
    
    int len_buffer = 10;
    char* buffer = mini_calloc(sizeof(char), len_buffer);
    if (buffer == NULL) mini_exit(1);

    mini_printf("1. Entrez un mot court (<10 caracteres): ");
    mini_scanf(buffer, len_buffer);
    mini_printf("   -> Lu: ");
    mini_printf(buffer);
    mini_printf("\n");

    mini_printf("2. Entrez une phrase longue (>10 caracteres): ");
    mini_scanf(buffer, len_buffer);
    mini_printf("   -> Lu (tronque): ");
    mini_printf(buffer);
    mini_printf("\n");

    mini_printf("3. Entrez un dernier mot (si 3. s'affiche => fonctione car buffer précedent vidé): ");
    mini_scanf(buffer, len_buffer);
    mini_printf("   -> Lu: ");
    mini_printf(buffer);
    mini_printf("\n");

    mini_free(buffer);
}


int main(int argc, char **argv) {
    mini_printf("\nDÉMARRAGE DES TESTS MINI-GLIBC\n");
    mini_printf("-----------------------------------------\n");

    test_basic_allocation();
    test_reusable_memory();
    test_mini_string();
    test_mini_scanf();

    mini_printf("\n-----------------------------------------\n");
    mini_printf("FIN DES TESTS.\n");
    
    mini_exit(0);
}