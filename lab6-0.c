#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int value;
    unsigned char height;
    struct node *left;
    struct node *right;
} node;

unsigned char height(node *tree) {
    return tree ? tree->height : 0;
}

void fix_height(node *tree) {
    unsigned char hl = height(tree->left);
    unsigned char hr = height(tree->right);
    tree->height = (hl > hr ? hl : hr) + 1;
}

node *rotate_left(node *root) {
    node *right = root->right;
    root->right = right->left;
    right->left = root;
    fix_height(root);
    fix_height(right);
    return right;
}

node *rotate_right(node *root) {
    node *left = root->left;
    root->left = left->right;
    left->right = root;
    fix_height(root);
    fix_height(left);
    return left;
}

int balance_factor(node *tree) {
    return height(tree->right) - height(tree->left);
}

node *balance(node *tree) {
    fix_height(tree);
    if (balance_factor(tree) == 2) {
        if (balance_factor(tree->right) < 0) {
            tree->right = rotate_right(tree->right);
        }
        return rotate_left(tree);
    } else if (balance_factor(tree) == -2) {
        if (balance_factor(tree->left) > 0) {
            tree->left = rotate_left(tree->left);
        }
        return rotate_right(tree);
    }
    return tree;
}

node *insert(node *tree, int value, node *avl_nodes) {
    if (!tree){
        avl_nodes->value = value;
        return avl_nodes;
    };
    if (value < tree->value) {
        tree->left = insert(tree->left, value, avl_nodes);
    } else {
        tree->right = insert(tree->right, value, avl_nodes);
    }
    return balance(tree);
}

int main() {
    int N;
    FILE *in = fopen("in.txt", "r");
    if (fscanf(in, "%d", &N)) {};
    node *tree = NULL;
    node *avl_nodes = (node *) malloc(N * sizeof (node));
    for (int i = 0; i < N; i++){
        avl_nodes[i].left = avl_nodes[i].right = 0;
        avl_nodes[i].value = 0;
        avl_nodes[i].height = 1;
    }

    for (int i = 0; i < N; i++) {
        int num;
        if (fscanf(in, "%d", &num)) {};
        tree = insert(tree, num, &avl_nodes[i]);
    }

    printf("%d", height(tree));
    free(avl_nodes);

    fclose(in);
    return 0;
}
