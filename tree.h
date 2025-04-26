#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h> // pre large_integer, queryperformancecounter, atd.

// definicia struktury uzla stromu
typedef struct tree_node {
    int numKeys;
    int keys[2];
    struct tree_node* children[3];
    struct tree_node* parent;
} tree_node;

// --- prototypy funkcii ---

// z tree_utils.c
tree_node* createTreeNode();
void sortThreeKeys(int keys[3]);
void SleepQuery(int microseconds);
void printTree(tree_node* root, int level);
int treeHeight(tree_node* root);
void printTreeHeight(tree_node* root);
void freeTree(tree_node* node);
int searchTree(tree_node* root, int value); // vyhladavanie

// z insert.c
int findDuplicate(tree_node* root, int value); // kontrola duplicity pri vkladani
tree_node* splitLeafNode(tree_node* leaf, int value);
tree_node* splitParentNode(tree_node* node, int newKey, int childIndex, tree_node* leftChild, tree_node* rightChild);
tree_node* insertTreeNode(tree_node* root, int value);

// z delete.c (vratane pomocnych)
tree_node* findNodeWithKey(tree_node* root, int key); // najde uzol s klucom pre delete
tree_node* findMinLeaf(tree_node* node);
int getChildIndex(tree_node* node);
tree_node* getLeftSibling(tree_node* parent, int nodeIndex);
tree_node* getRightSibling(tree_node* parent, int nodeIndex);
void removeFromParentAfterMerge(tree_node* parent, int parentKeyIndexToRemove, int childToRemoveIndex);
tree_node* handleUnderflow(tree_node* node, tree_node* root);
tree_node* deleteTreeNode(tree_node* root, int key);


#endif // TREE_H