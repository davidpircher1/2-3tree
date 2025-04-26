#include "tree.h"

int findDuplicate(tree_node* node, int value) {
    if (!node) return 0;
    SleepQuery(1);
    for (int i = 0; i < node->numKeys; ++i) {
        if (node->keys[i] == value) {
            return 1;
        }
    }
    return 0;
}

tree_node* splitParentNode(tree_node* node, int promoteKey, int childIndex, tree_node* leftChild, tree_node* rightChild) {
    tree_node* tempChildren[4];
    switch (childIndex) {
    case 0:
        tempChildren[0] = leftChild; tempChildren[1] = rightChild;
        tempChildren[2] = node->children[1]; tempChildren[3] = node->children[2];
        break;
    case 1:
        tempChildren[0] = node->children[0]; tempChildren[1] = leftChild;
        tempChildren[2] = rightChild; tempChildren[3] = node->children[2];
        break;
    case 2:
        tempChildren[0] = node->children[0]; tempChildren[1] = node->children[1];
        tempChildren[2] = leftChild; tempChildren[3] = rightChild;
        break;
    default: return NULL;
    }

    int allKeys[3] = { node->keys[0], node->keys[1], promoteKey };
    sortThreeKeys(allKeys);
    int promoteUp = allKeys[1];

    tree_node* newLeftInternal = createTreeNode();
    tree_node* newRightInternal = createTreeNode();
    if (!newLeftInternal || !newRightInternal) {
        free(newLeftInternal);
        free(newRightInternal);
        return NULL;
    }

    newLeftInternal->keys[0] = allKeys[0];
    newLeftInternal->numKeys = 1;
    newRightInternal->keys[0] = allKeys[2];
    newRightInternal->numKeys = 1;

    newLeftInternal->children[0] = tempChildren[0];
    newLeftInternal->children[1] = tempChildren[1];
    newRightInternal->children[0] = tempChildren[2];
    newRightInternal->children[1] = tempChildren[3];

    if (tempChildren[0]) tempChildren[0]->parent = newLeftInternal;
    if (tempChildren[1]) tempChildren[1]->parent = newLeftInternal;
    if (tempChildren[2]) tempChildren[2]->parent = newRightInternal;
    if (tempChildren[3]) tempChildren[3]->parent = newRightInternal;

    if (node->parent == NULL) {
        tree_node* newRoot = createTreeNode();
        if (!newRoot) {
            free(newLeftInternal); free(newRightInternal);
            return node;
        }
        newRoot->keys[0] = promoteUp;
        newRoot->numKeys = 1;
        newRoot->children[0] = newLeftInternal;
        newRoot->children[1] = newRightInternal;
        newLeftInternal->parent = newRoot;
        newRightInternal->parent = newRoot;
        free(node);
        return newRoot;
    }
    else {
        tree_node* parent = node->parent;
        int parentIndex = getChildIndex(node);
        if (parentIndex == -1) {
            free(newLeftInternal); free(newRightInternal);
            tree_node* root = node; while (root && root->parent) root = root->parent; return root;
        }

        free(node);
        node = NULL;

        if (parent->numKeys == 1) {
            if (parentIndex == 0) {
                parent->keys[1] = parent->keys[0];
                parent->keys[0] = promoteUp;
                parent->children[2] = parent->children[1];
                parent->children[0] = newLeftInternal;
                parent->children[1] = newRightInternal;
            }
            else {
                parent->keys[1] = promoteUp;
                parent->children[1] = newLeftInternal;
                parent->children[2] = newRightInternal;
            }
            parent->numKeys = 2;
            newLeftInternal->parent = parent;
            newRightInternal->parent = parent;
            tree_node* root = parent; while (root->parent) root = root->parent;
            return root;
        }
        else {
            return splitParentNode(parent, promoteUp, parentIndex, newLeftInternal, newRightInternal);
        }
    }
}

tree_node* splitLeafNode(tree_node* leaf, int value) {
    int allKeys[3] = { leaf->keys[0], leaf->keys[1], value };
    sortThreeKeys(allKeys);

    const int promoteKey = allKeys[1];

    tree_node* leftLeaf = createTreeNode();
    tree_node* rightLeaf = createTreeNode();
    if (!leftLeaf || !rightLeaf) {
        free(leftLeaf); free(rightLeaf);
        tree_node* root = leaf; while (root && root->parent) root = root->parent; return root;
    }

    leftLeaf->keys[0] = allKeys[0];
    leftLeaf->numKeys = 1;
    rightLeaf->keys[0] = allKeys[2];
    rightLeaf->numKeys = 1;

    if (leaf->parent == NULL) {
        tree_node* newRoot = createTreeNode();
        if (!newRoot) {
            free(leftLeaf); free(rightLeaf);
            return leaf;
        }
        newRoot->keys[0] = promoteKey;
        newRoot->numKeys = 1;
        newRoot->children[0] = leftLeaf;
        newRoot->children[1] = rightLeaf;
        leftLeaf->parent = newRoot;
        rightLeaf->parent = newRoot;
        free(leaf);
        return newRoot;
    }
    else {
        tree_node* parent = leaf->parent;
        int parentIndex = getChildIndex(leaf);
        if (parentIndex == -1) {
            free(leftLeaf); free(rightLeaf);
            tree_node* root = leaf; while (root && root->parent) root = root->parent; return root;
        }

        free(leaf);
        leaf = NULL;

        if (parent->numKeys == 1) {
            if (parentIndex == 0) {
                parent->keys[1] = parent->keys[0];
                parent->keys[0] = promoteKey;
                parent->children[2] = parent->children[1];
                parent->children[0] = leftLeaf;
                parent->children[1] = rightLeaf;
            }
            else {
                parent->keys[1] = promoteKey;
                parent->children[1] = leftLeaf;
                parent->children[2] = rightLeaf;
            }
            parent->numKeys = 2;
            leftLeaf->parent = parent;
            rightLeaf->parent = parent;
            tree_node* root = parent; while (root->parent) root = root->parent;
            return root;
        }
        else {
            return splitParentNode(parent, promoteKey, parentIndex, leftLeaf, rightLeaf);
        }
    }
}

tree_node* insertTreeNode(tree_node* root, int value) {
    if (root == NULL) {
        root = createTreeNode();
        if (!root) {
            return NULL;
        }
        root->keys[0] = value;
        root->numKeys = 1;
        return root;
    }

    tree_node* current = root;
    while (current->children[0] != NULL) {
        if (findDuplicate(current, value)) {
            return root;
        }

        if (current->numKeys == 1) {
            current = (value < current->keys[0]) ? current->children[0] : current->children[1];
        }
        else {
            current = (value < current->keys[0]) ? current->children[0] :
                (value < current->keys[1]) ? current->children[1] :
                current->children[2];
        }
        if (current == NULL) {
            return root;
        }
    }

    if (findDuplicate(current, value)) {
        return root;
    }

    if (current->numKeys < 2) {
        if (current->numKeys == 0) {
            current->keys[0] = value;
        }
        else if (value < current->keys[0]) {
            current->keys[1] = current->keys[0];
            current->keys[0] = value;
        }
        else {
            current->keys[1] = value;
        }
        current->numKeys++;
        return root;
    }
    else {
        tree_node* potentialNewRoot = splitLeafNode(current, value);
        return potentialNewRoot ? potentialNewRoot : root;
    }
}