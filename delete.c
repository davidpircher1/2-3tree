#include "tree.h"

tree_node* findNodeWithKey(tree_node* root, int key) {
    if (root == NULL) {
        return NULL;
    }
    tree_node* current = root;
    while (current != NULL) {
        SleepQuery(1);

        // pozrieme kluce
        for (int i = 0; i < current->numKeys; ++i) {
            if (key == current->keys[i]) return current;
        }
        if (current->children[0] == NULL) return NULL; // nenasli sme

        // pohyb
        if (key < current->keys[0]) {
            current = current->children[0];
        }
        else if (current->numKeys == 1) {
            current = current->children[1];
        }
        else {
            current = (key < current->keys[1]) ? current->children[1] : current->children[2]; 
        }
    }
    return NULL;
}

tree_node* findMinLeaf(tree_node* node) {
    if (node == NULL) return NULL; // prazdne
    tree_node* current = node;
    while (current->children[0] != NULL) {
        SleepQuery(1);
        current = current->children[0]; // hladame najemnsi uzol posun dolava
    }
    return current;
}

int getChildIndex(tree_node* node) {
    if (node == NULL || node->parent == NULL) return -1;
    tree_node* parent = node->parent;
    if (parent->children[0] == node) return 0;
    if (parent->children[1] == node) return 1;
    if (parent->children[2] == node) return 2;
    return -1;
}

tree_node* getLeftSibling(tree_node* parent, int nodeIndex) {
    if (parent == NULL || nodeIndex <= 0 || nodeIndex > 2) {
        return NULL;
    }
    if (nodeIndex == 1) return parent->children[0];
    if (nodeIndex == 2) {
        if (parent->numKeys == 2) return parent->children[1];
        else return NULL;
    }
    return NULL;
}

tree_node* getRightSibling(tree_node* parent, int nodeIndex) {
    if (parent == NULL || nodeIndex < 0 || nodeIndex > 2) { // Index 2 cannot have a right sibling
        return NULL;
    }
    if (nodeIndex == 0) {
        return parent->children[1];
    }
    if (nodeIndex == 1) {
        return (parent->numKeys == 2) ? parent->children[2] : NULL;
    }
    return NULL;
}

void removeFromParentAfterMerge(tree_node* parent, int parentKeyIndexToRemove, int childPointerIndexToRemove) {
    if (!parent) return;

    for (int i = parentKeyIndexToRemove; i < parent->numKeys - 1; ++i) {
        parent->keys[i] = parent->keys[i + 1];
    }
    if (parent->numKeys > 0) {
        parent->keys[parent->numKeys - 1] = 0;
    }

    tree_node* temp_children[3] = { NULL, NULL, NULL };
    int k = 0;
    for (int i = 0; i < 3; ++i) {
        if (i == childPointerIndexToRemove) continue;
        temp_children[k++] = parent->children[i];
    }

    parent->children[0] = temp_children[0];
    parent->children[1] = temp_children[1];
    parent->children[2] = temp_children[2]; // k will be max 2, so temp_children[2] is NULL

    parent->numKeys--;
}


tree_node* handleUnderflow(tree_node* node, tree_node* root) {
    SleepQuery(1);

    if (node->parent == NULL) {
        if (node->numKeys == 0) {
            if (node->children[0] == NULL) {
                free(node);
                return NULL;
            }
            else {
                tree_node* newRoot = node->children[0];
                if (newRoot) newRoot->parent = NULL;
                free(node);
                return newRoot;
            }
        }
        return node;
    }

    tree_node* parent = node->parent;
    int nodeIndex = getChildIndex(node);

    if (nodeIndex == -1) {
        return root;
    }

    tree_node* leftSibling = getLeftSibling(parent, nodeIndex);
    tree_node* rightSibling = getRightSibling(parent, nodeIndex);

    if (leftSibling != NULL && leftSibling->numKeys == 2) { // pozicanie z laveho surodenca
        int parentKeyIndex = (nodeIndex == 1 || nodeIndex == 2) ? nodeIndex - 1 : 0; // correct index

        int keyFromParent = parent->keys[parentKeyIndex];
        int keyToParent = leftSibling->keys[1];
        tree_node* childToNode = leftSibling->children[2];

        node->keys[0] = keyFromParent;
        node->numKeys = 1;
        node->children[1] = node->children[0];
        node->children[0] = childToNode;
        if (childToNode) childToNode->parent = node;

        parent->keys[parentKeyIndex] = keyToParent;

        leftSibling->keys[1] = 0;
        leftSibling->children[2] = NULL;
        leftSibling->numKeys = 1;

        return root;
    }
    else if (rightSibling != NULL && rightSibling->numKeys == 2) { // pozicat z praveho surodenca
        int parentKeyIndex = nodeIndex;

        int keyFromParent = parent->keys[parentKeyIndex];
        int keyToParent = rightSibling->keys[0];
        tree_node* childToNode = rightSibling->children[0];

        node->keys[0] = keyFromParent;
        node->numKeys = 1;
        node->children[1] = childToNode;
        node->children[2] = NULL;
        if (childToNode) childToNode->parent = node;

        parent->keys[parentKeyIndex] = keyToParent;

        rightSibling->keys[0] = rightSibling->keys[1];
        rightSibling->keys[1] = 0;
        rightSibling->children[0] = rightSibling->children[1];
        rightSibling->children[1] = rightSibling->children[2];
        rightSibling->children[2] = NULL;
        rightSibling->numKeys = 1;
        if (rightSibling->children[0]) rightSibling->children[0]->parent = rightSibling;
        if (rightSibling->children[1]) rightSibling->children[1]->parent = rightSibling; // Update rodica pre posunutue dieta [1]

        return root;
    }
    else { // nevieme si pozicat musime spajat
        if (leftSibling != NULL) { // spojenie s lavym surodencom
            tree_node* targetNode = leftSibling;
            tree_node* nodeToRemove = node;
            int parentKeyIndex = (nodeIndex == 1 || nodeIndex == 2) ? nodeIndex - 1 : 0;
            int keyFromParent = parent->keys[parentKeyIndex];

            targetNode->keys[1] = keyFromParent;
            targetNode->numKeys = 2;
            targetNode->children[2] = nodeToRemove->children[0];
            if (targetNode->children[2]) targetNode->children[2]->parent = targetNode;

            int childPointerIndexToRemove = nodeIndex;
            tree_node* parentForRecursion = parent;
            removeFromParentAfterMerge(parent, parentKeyIndex, childPointerIndexToRemove);
            free(nodeToRemove);

            if (parentForRecursion->numKeys == 0 && parentForRecursion != root) {
                return handleUnderflow(parentForRecursion, root);
            }
            else if (parentForRecursion->numKeys == 0 && parentForRecursion == root) {
                targetNode->parent = NULL;
                free(parentForRecursion);
                return targetNode;
            }
            else {
                return root;
            }
        }
        else if (rightSibling != NULL) { // spojenie s pravym uzlom
            tree_node* targetNode = node;
            tree_node* nodeToRemove = rightSibling;
            int parentKeyIndex = nodeIndex;
            int keyFromParent = parent->keys[parentKeyIndex];

            targetNode->keys[0] = keyFromParent;
            targetNode->keys[1] = nodeToRemove->keys[0];
            targetNode->numKeys = 2;
            targetNode->children[1] = nodeToRemove->children[0];
            targetNode->children[2] = nodeToRemove->children[1];
            if (targetNode->children[1]) targetNode->children[1]->parent = targetNode;
            if (targetNode->children[2]) targetNode->children[2]->parent = targetNode;

            int childPointerIndexToRemove = getChildIndex(nodeToRemove);
            if (childPointerIndexToRemove == -1) {
                return root; // Error case
            }
            tree_node* parentForRecursion = parent;
            removeFromParentAfterMerge(parent, parentKeyIndex, childPointerIndexToRemove);
            free(nodeToRemove);

            if (parentForRecursion->numKeys == 0 && parentForRecursion != root) {
                return handleUnderflow(parentForRecursion, root);
            }
            else if (parentForRecursion->numKeys == 0 && parentForRecursion == root) {
                targetNode->parent = NULL;
                free(parentForRecursion);
                return targetNode;
            }
            else {
                return root;
            }
        }
        else {
            return root; // chyba nema nastat
        }
    }
}

tree_node* deleteTreeNode(tree_node* root, int key) {
    if (root == NULL) return NULL;

    tree_node* nodeWithKey = findNodeWithKey(root, key);

    if (nodeWithKey == NULL) {
        return root;
    }

    tree_node* nodeToRemoveFrom;
    int keyToRemove;

    if (nodeWithKey->children[0] != NULL) {
        tree_node* successorSubtree = NULL;
        int keyIndexInNode = (key == nodeWithKey->keys[0]) ? 0 : 1;

        if (nodeWithKey->numKeys == 1) {
            successorSubtree = nodeWithKey->children[1]; // Fixed: index 1 for 2-node
        }
        else {
            successorSubtree = nodeWithKey->children[keyIndexInNode + 1]; // child[1] or child[2]
        }

        tree_node* successorLeaf = findMinLeaf(successorSubtree);
        if (!successorLeaf) {
            return root; // Should not happen
        }
        int successorKey = successorLeaf->keys[0];

        nodeWithKey->keys[keyIndexInNode] = successorKey;

        nodeToRemoveFrom = successorLeaf;
        keyToRemove = successorKey;
    }
    else {
        nodeToRemoveFrom = nodeWithKey;
        keyToRemove = key;
    }

    if (nodeToRemoveFrom->numKeys == 2) {
        if (nodeToRemoveFrom->keys[0] == keyToRemove) {
            nodeToRemoveFrom->keys[0] = nodeToRemoveFrom->keys[1];
        }
        nodeToRemoveFrom->keys[1] = 0;
        nodeToRemoveFrom->numKeys--;
        return root;
    }
    else {
        nodeToRemoveFrom->keys[0] = 0;
        nodeToRemoveFrom->numKeys--;
        root = handleUnderflow(nodeToRemoveFrom, root);
        return root;
    }
}