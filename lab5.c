#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH_ALPHABET 256
#define SIZE_OF_BYTE 8
#define MAX_BUFFER_LEN 1000000

typedef struct treeNode {
    unsigned char symbol;
    unsigned int weight;
    struct treeNode *left, *right;
} treeNode;

typedef struct {
    treeNode *queue[MAX_LENGTH_ALPHABET];
    int front, rear;
} priorityQueue;


typedef struct {
    unsigned char *buf;
    unsigned int byteCount, biteCount, bufSize;
} packedData;

priorityQueue *createQueue() {
    priorityQueue *priorityQueue1 = malloc(sizeof(priorityQueue));
    priorityQueue1->front = priorityQueue1->rear = -1;
    return priorityQueue1;
}

void insertQueue(priorityQueue *priorityQueue1, treeNode *node) {
    if (priorityQueue1->front == -1 && priorityQueue1->rear == -1) {
        priorityQueue1->front++, priorityQueue1->rear++;
        priorityQueue1->queue[priorityQueue1->front] = node;
        return;
    }
    int i;
    for (i = 0; i <= priorityQueue1->rear; i++) {
        if (node->weight >= priorityQueue1->queue[i]->weight) {
            for (int j = priorityQueue1->rear + 1; j > i; j--) {
                priorityQueue1->queue[j] = priorityQueue1->queue[j - 1];
            }
            priorityQueue1->queue[i] = node;
            priorityQueue1->rear++;
            return;
        }
    }
    priorityQueue1->rear++;
    priorityQueue1->queue[i] = node;
}

treeNode *getLeastNode(priorityQueue *priorityQueue1) {
    return priorityQueue1->queue[priorityQueue1->rear--];
}

treeNode *createTreeNode(unsigned char symbol, unsigned int weight, treeNode *left, treeNode *right) {
    treeNode *tempNode = malloc(sizeof(treeNode));
    tempNode->symbol = symbol, tempNode->weight = weight;
    tempNode->left = left, tempNode->right = right;
    return tempNode;
}

void makeHuffmanTree(treeNode **root, unsigned int *stat) {
    priorityQueue *priorityQueue1 = createQueue();
    for (int i = 0; i < MAX_LENGTH_ALPHABET; i++) {
        if (stat[i]) {
            insertQueue(priorityQueue1, createTreeNode(i, stat[i], 0, 0));
        }
    }
    while ((priorityQueue1->rear - priorityQueue1->front) > 0) {
        treeNode *node1 = getLeastNode(priorityQueue1), *node2 = getLeastNode(priorityQueue1);
        insertQueue(priorityQueue1, createTreeNode(0, node1->weight + node2->weight, node1, node2));
    }
    treeNode *tempNode = getLeastNode(priorityQueue1);
    *root = tempNode;
    free(priorityQueue1);
}

void removeHuffmanTree(treeNode **root) {
    if ((*root)->left != 0) {
        removeHuffmanTree((&(*root)->left));
        removeHuffmanTree(&((*root)->right));
    }
    free((*root));
    (*root) = 0;
}

long readInputFile(unsigned int *stat, FILE *file) {
    unsigned char currentSymbol;
    long strLength = 0;
    for (int i = 0; i < 256; i++) {
        stat[i] = 0;
    }

    while (fread(&currentSymbol, 1, 1, file)) {
        stat[(int) currentSymbol] += 1;
        strLength++;
    }
    fseek(file, 1, SEEK_SET);
    return strLength;
}

void treeToTable(treeNode *root, char **codes, char *curCode) {
    size_t len = strlen(curCode);
    if (!root->left) {
        codes[root->symbol] = (char *) malloc(len + 1);
        strcpy(codes[root->symbol], curCode);
    } else {
        curCode[len] = '0';
        curCode[len + 1] = '\0';
        if (root->left) treeToTable(root->left, codes, curCode);
        curCode[len] = '1';
        curCode[len + 1] = '\0';
        if (root->right) treeToTable(root->right, codes, curCode);
    }
}

void removeTableCodes(char **table){
    for (int i = 0; i < MAX_LENGTH_ALPHABET; i++){
        if (table[i]) free(table[i]);
    }
}

void initPackageData(packedData *package, unsigned int byteCount) {
    package->bufSize = byteCount, package->biteCount = 8, package->byteCount = 0;
    package->buf = (unsigned char *) malloc(sizeof(unsigned char) * byteCount);
    for (unsigned int i = 0; i < byteCount; i++) {
        package->buf[i] = 0;
    }
}

void bitWrite(FILE *file, packedData *package, unsigned char bit) {
    package->buf[package->byteCount] |= ((bit - '0') << (package->biteCount - 1));
    package->biteCount--;
    if (package->biteCount == 0) {
        fwrite(&package->buf[package->byteCount], sizeof(unsigned char), 1, file);
        package->biteCount = 8;
        package->buf[package->byteCount] = 0;
    }
}

int unpackBit(packedData *package, unsigned char *bit) {
    if (package->byteCount < package->bufSize) {
        *bit = (package->buf[package->byteCount] >> (package->biteCount - 1)) & 1;
        package->biteCount--;
        if (package->biteCount == 0) {
            package->byteCount++;
            package->biteCount = 8;
        }
        return 0;
    } else return 1;
}

void unpackByte(packedData *package, unsigned char *byte) {
    *byte = package->buf[package->byteCount] << (8 - package->biteCount);
    package->byteCount++;
    *byte |= package->buf[package->byteCount] >> package->biteCount;
}

unsigned char unpackingChar(FILE *file, treeNode *root, packedData *package) {
    if (root->left) {
        unsigned char bit = 0;
        if (unpackBit(package, &bit)) {
            if (fread(package->buf, 1, MAX_BUFFER_LEN, file)) {};
            package->byteCount = 0;
            unpackBit(package, &bit);
        }
        if (bit) {
            return unpackingChar(file, root->right, package);
        }
        else {
            return unpackingChar(file, root->left, package);
        }
    } else {
        return root->symbol;
    }
}

void packHuffmanTree(FILE *file, packedData *package, treeNode *root) {
    if (root->left == 0) {
        bitWrite(file, package, '0');
        for (int i = 0; i < SIZE_OF_BYTE; i++){
            bitWrite(file, package, ((root->symbol) >> (7 - i) & 1) + '0');
        }

    } else {
        bitWrite(file, package, '1');
        packHuffmanTree(file, package, root->left);
        packHuffmanTree(file, package, root->right);
    }
}

void unpackHuffmanTree(packedData *package, treeNode **root) {
    *root = (treeNode *) malloc(sizeof(treeNode));
    (*root)->weight = 0;
    unsigned char bit = 0;
    unpackBit(package, &bit);
    if (bit) {
        (*root)->symbol = 0;
        unpackHuffmanTree(package, &((*root)->left));
        unpackHuffmanTree(package, &((*root)->right));
    } else {
        (*root)->left = 0, (*root)->right = 0;
        unsigned char symbol;
        unpackByte(package, &symbol);
        (*root)->symbol = symbol;
    }
}

void encode(FILE *in, FILE *out) {
    unsigned int tableFreguency[256];
    long textLength = readInputFile(tableFreguency, in);
    if (!textLength) return;
    treeNode *root;
    makeHuffmanTree(&root, tableFreguency);
    char* codes[MAX_LENGTH_ALPHABET] = {NULL};
    char curCode[MAX_LENGTH_ALPHABET] = {0};
    treeToTable(root, codes, curCode);
    fwrite(&(textLength), 4, 1, out);
    packedData package;
    initPackageData(&package, 1);
    packHuffmanTree(out, &package, root);
    for (long i = 0; i < textLength; i++){
        unsigned char c;
        if (fread(&c, 1, 1, in)){}
        for (unsigned int j = 0; j < strlen(codes[c]); j++){
            bitWrite(out, &package, codes[c][j]);
        }
    }
    if (package.biteCount != SIZE_OF_BYTE) {
        fwrite(&(package.buf[package.byteCount]), 1, 1, out);
    }
    free(package.buf);
    removeHuffmanTree(&root);
    removeTableCodes(codes);
}

void decode(FILE *file, FILE *out) {
    treeNode *root;
    unsigned int numberOfEncodedChar = 0;
    packedData packageData;
    initPackageData(&packageData, MAX_BUFFER_LEN);
    if (fread(&numberOfEncodedChar, 4, 1, file)) {};
    if (fread(packageData.buf, 1, MAX_BUFFER_LEN, file)) {};
    unpackHuffmanTree(&packageData, &root);
    for (unsigned long i = 0; i < numberOfEncodedChar; i++) {
        unsigned char symbol = unpackingChar(file, root, &packageData);
        fwrite(&symbol, 1, 1, out);
    }
    free(packageData.buf);
    removeHuffmanTree(&root);
}

int main() {
    unsigned char c;
    FILE *in = fopen("in.txt", "rb");
    FILE *out = fopen("out.txt", "wb");
    if (fread(&c, 1, 1, in)) {};

    if (c == 'c') encode(in, out);
    else decode(in, out);

    fclose(in);
    fclose(out);
    return 0;
}
