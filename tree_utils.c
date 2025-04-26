#include "tree.h"

// vytvori novy uzol stromu, inicializuje na 0/null
tree_node* createTreeNode() {
    tree_node* newNode = (tree_node*)calloc(1, sizeof(tree_node));
    if (newNode == NULL) {
        return NULL;
    }
    newNode->numKeys = 0;
    newNode->parent = NULL;
    return newNode;
}

// utriedi pole troch int hodnot
void sortThreeKeys(int keys[3]) {
    if (keys[0] > keys[1]) {
        int temp = keys[0];
        keys[0] = keys[1];
        keys[1] = temp;
    }
    if (keys[1] > keys[2]) {
        int temp = keys[1];
        keys[1] = keys[2];
        keys[2] = temp;
    }
    if (keys[0] > keys[1]) {
        int temp = keys[0];
        keys[0] = keys[1];
        keys[1] = temp;
    }
}

// funkcia na cakanie (windows api)
void SleepQuery(int microseconds) {
    LARGE_INTEGER frequency, start, current;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    double targetTicks = (microseconds / 1e6) * frequency.QuadPart;

    do {
        QueryPerformanceCounter(&current);
    } while ((current.QuadPart - start.QuadPart) < targetTicks);
}

// rekurzivne vypise strom podla urovni
void printTree(tree_node* root, int level) {
    if (root == NULL) return;
    printf("level %d: ", level);
    for (int i = 0; i < root->numKeys; i++) {
        printf("%d ", root->keys[i]);
    }
    printf("\n");
    for (int i = 0; i < 3; i++) {
        if (root->children[i] != NULL) {
            printTree(root->children[i], level + 1);
        }
    }
}

// vrati vysku stromu
int treeHeight(tree_node* root) {
    int height = 1;
    tree_node* current = root;
    while (current != NULL) {
        height++;
        current = current->children[0]; // prejdeme k lavému podstromu
    }
    return height;
}

// vypise vysku stromu
void printTreeHeight(tree_node* root) {
    int height = treeHeight(root);
    printf("vyska stromu: %d\n", height);
}

// uvolnenie celeho stromu
void freeTree(tree_node* node) {
    if (node == NULL) return;
    // rekurzivne uvolni deti
    freeTree(node->children[0]);
    freeTree(node->children[1]);
    freeTree(node->children[2]);
    // uvolni uzol
    free(node);
}

// vyhladavanie kluca v strome
int searchTree(tree_node* root, int value) {
    if (root == NULL) return 0; // prazdny strom

    tree_node* current = root;

    while (current != NULL) {
        SleepQuery(3); // simulacia zataze

        // prehladanie klucov v aktualnom uzle
        for (int i = 0; i < current->numKeys; i++) {
            if (current->keys[i] == value) {
                return 1; // najdene
            }
        }

        // ak sme v liste a nenasli sme, koniec
        if (current->children[0] == NULL) {
            return 0; // nenasli sme v liste
        }

        // rozhodni sa, do ktoreho dietata ist dalej
        if (value < current->keys[0]) {
            current = current->children[0]; // chod dolava
        }
        else if (current->numKeys == 1 || value < current->keys[1]) {
            current = (current->numKeys == 1) ? current->children[2] : current->children[1];
        }
        else { // value >= current->keys[1] (plati len ak numkeys == 2)
            current = current->children[2]; // chod doprava
        }
    }

    return 0; // ak current == null, hodnota sa nenasla
}