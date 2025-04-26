#define _CRT_SECURE_NO_WARNINGS
#include "tree.h" // obsahuje vsetky potrebne include a prototypy

// konstanty pre meranie
#define TOTAL_ELEMENTS 1000000
#define BLOCK_SIZE 1000


int main() { // Pridal som main funkciu pre kontext
    tree_node* root = NULL;
    LARGE_INTEGER frequency;
    LARGE_INTEGER block_start_time; // cas zaciatku aktualneho bloku
    LARGE_INTEGER end_time;         // koncovy cas pre vypocet bloku
    double block_time_microseconds;
    double avg_op_time_microseconds;

    QueryPerformanceFrequency(&frequency);
    srand((unsigned int)time(NULL)); // Inicializacia generatora nahodnych cisel

    // Otvorenie suborov pre povodne (sekvencne) merania
    FILE* f_insert_seq = fopen("insert_times_block.txt", "w");
    FILE* f_search_seq = fopen("search_times_block.txt", "w");
    FILE* f_delete_seq = fopen("delete_times_block.txt", "w");

    // --- Faza 1: Povodne meranie insert (sekvencne) ---
    printf("Faza 1: Meranie insert (sekvencne)...\n");
    QueryPerformanceCounter(&block_start_time);
    for (int i = 1; i <= TOTAL_ELEMENTS; ++i) {
        root = insertTreeNode(root, i); // vkladame 1, 2, 3,...
        if (i % BLOCK_SIZE == 0) {
            QueryPerformanceCounter(&end_time);
            block_time_microseconds = (double)(end_time.QuadPart - block_start_time.QuadPart) * 1e6 / frequency.QuadPart;
            avg_op_time_microseconds = block_time_microseconds / BLOCK_SIZE;
            fprintf(f_insert_seq, "%.10f\n", avg_op_time_microseconds);
            block_start_time = end_time;
        }
    }
    printf("Sekvencny insert dokonceny.\n");
    printTreeHeight(root);

    // --- Faza 2: Povodne meranie search (sekvencne) ---
    printf("Faza 2: Meranie search (sekvencne)...\n");
    QueryPerformanceCounter(&block_start_time);
    for (int i = 300000; i <= TOTAL_ELEMENTS + 500000; ++i) {
        searchTree(root, i);
        if (i % BLOCK_SIZE == 0) {
            QueryPerformanceCounter(&end_time);
            block_time_microseconds = (double)(end_time.QuadPart - block_start_time.QuadPart) * 1e6 / frequency.QuadPart;
            avg_op_time_microseconds = block_time_microseconds / BLOCK_SIZE;
            fprintf(f_search_seq, "%.10f\n", avg_op_time_microseconds);
            block_start_time = end_time;
        }
    }
    printf("Sekvencny search dokonceny.\n");

    // --- Faza 3: Povodne meranie delete (sekvencne) ---
    printf("Faza 3: Meranie delete (sekvencne)...\n");
    QueryPerformanceCounter(&block_start_time);
    for (int i = 1; i <= TOTAL_ELEMENTS; ++i) {
        int key_to_delete = i;
        root = deleteTreeNode(root, key_to_delete);
        if (i % BLOCK_SIZE == 0) {
            QueryPerformanceCounter(&end_time);
            block_time_microseconds = (double)(end_time.QuadPart - block_start_time.QuadPart) * 1e6 / frequency.QuadPart;
            avg_op_time_microseconds = block_time_microseconds / BLOCK_SIZE;
            fprintf(f_delete_seq, "%.10f\n", avg_op_time_microseconds);
            block_start_time = end_time;
        }
    }
    printf("Sekvencny delete dokonceny.\n");
    printf("Povodne (sekvencne) merania dokoncene.\n");
    printTreeHeight(root);


    // Zatvorenie vsetkych suborov
    fclose(f_insert_seq);
    fclose(f_search_seq);
    fclose(f_delete_seq);


    freeTree(root);
    root = NULL;
   
    FILE* f_insert_rand = fopen("insert_times_block_random.txt", "w");
    FILE* f_search_rand = fopen("search_times_block_random.txt", "w");
    FILE* f_delete_rand = fopen("delete_times_block_random.txt", "w");

   printf("\nSpustam nove merania (nahodne)...\n");
    // --- Faza 4: Nove meranie insert (nahodne) ---
    printf("Faza 4: Meranie insert (nahodne)...\n");
    QueryPerformanceCounter(&block_start_time);
    for (int i = 1; i <= TOTAL_ELEMENTS; ++i) {
        int random_key = (rand() << 15 | rand()) % TOTAL_ELEMENTS + 1;
        root = insertTreeNode(root, random_key); // Vkladame nahodne cislo
        if (i % BLOCK_SIZE == 0) {
            QueryPerformanceCounter(&end_time);
            block_time_microseconds = (double)(end_time.QuadPart - block_start_time.QuadPart) * 1e6 / frequency.QuadPart;
            avg_op_time_microseconds = block_time_microseconds / BLOCK_SIZE;
            fprintf(f_insert_rand, "%.10f\n", avg_op_time_microseconds);
            block_start_time = end_time;
        }
    }
    printf("Nahodny insert dokonceny.\n");
    printTreeHeight(root);

    // --- Faza 5: Nove meranie search (nahodne) ---
    printf("Faza 5: Meranie search (nahodne)...\n");
    QueryPerformanceCounter(&block_start_time);
    for (int i = 1; i <= TOTAL_ELEMENTS; ++i) { // Pouzivame i len ako pocitadlo operacii
        int random_key_to_search = (rand() << 15 | rand()) % 10000000 + 1;
        searchTree(root, random_key_to_search); // Hladame nahodne cislo
        if (i % BLOCK_SIZE == 0) {
            QueryPerformanceCounter(&end_time);
            block_time_microseconds = (double)(end_time.QuadPart - block_start_time.QuadPart) * 1e6 / frequency.QuadPart;
            avg_op_time_microseconds = block_time_microseconds / BLOCK_SIZE;
            fprintf(f_search_rand, "%.10f\n", avg_op_time_microseconds);
            block_start_time = end_time;
        }
    }
    printf("Nahodny search dokonceny.\n");

    // --- Faza 6: Nove meranie delete (nahodne) ---
    printf("Faza 6: Meranie delete (nahodne)...\n");
    QueryPerformanceCounter(&block_start_time);
    for (int i = 1; i <= TOTAL_ELEMENTS; ++i) { // Pouzivame i len ako pocitadlo operacii
        int key_to_delete = (rand() << 15 | rand()) % TOTAL_ELEMENTS + 1;
        root = deleteTreeNode(root, key_to_delete); // Mazeme nahodne cislo
        if (i % BLOCK_SIZE == 0) {
            QueryPerformanceCounter(&end_time);
            block_time_microseconds = (double)(end_time.QuadPart - block_start_time.QuadPart) * 1e6 / frequency.QuadPart;
            avg_op_time_microseconds = block_time_microseconds / BLOCK_SIZE;
            fprintf(f_delete_rand, "%.10f\n", avg_op_time_microseconds);
            block_start_time = end_time;
        }
    }
    printf("Nahodny delete dokonceny.\n");
    printf("Vsetky merania dokoncene.");
    printTreeHeight(root);

    // Zatvorenie vsetkych suborov
    fclose(f_insert_rand);
    fclose(f_search_rand);
    fclose(f_delete_rand);

    //free tree
    freeTree(root);

    return 0;
}