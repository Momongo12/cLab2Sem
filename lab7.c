#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_VERTICES 2000
#define BITS (unsigned int) (sizeof(unsigned int)*8)

typedef struct {
    int verticesNumber, edgeNumber;
    unsigned int *matrix;
} Graph;

typedef struct {
    int size;
    unsigned short *data;
}Stack;

bool checkInput(int verticesNumber, int edgeNumber);
bool checkNumberOfVertices(unsigned short src, unsigned short dest, int verticesNumber);
Graph *createGraph(int verticesNumber, int edgeNumber);
void destroyGraph(Graph *graph);
bool getGraph(FILE *in, Graph *graph);
bool dfs(Graph *graph, unsigned short startVertex, unsigned char *colorOfVertices, Stack *result);
bool topologicalSort(Graph *graph);
Stack *createStack(int size);
void destroyStack(Stack *stack);
void pushStack(Stack *stack, unsigned short value);
unsigned short popStack(Stack *stack);


int main() {
    int N, M;
    FILE *in = fopen("in.txt", "r");
    if ((fscanf(in, "%d", &N) < 1) || (fscanf(in, "%d", &M) < 1)) {
        printf("bad number of lines");
        fclose(in);
        return 0;
    }

    if (!checkInput(N, M)) {
        fclose(in);
        return 0;
    }

    Graph *graph = createGraph(N, M);
    if (!getGraph(in, graph)) {
        destroyGraph(graph);
        fclose(in);
        return 0;
    }
    if (!topologicalSort(graph)) {
        destroyGraph(graph);
        fclose(in);
        return 0;
    }

    destroyGraph(graph);
    fclose(in);
    return 0;
}

Stack *createStack(int size){
    Stack *stack = (Stack *) malloc(sizeof(Stack));
    stack->size = 0;
    stack->data = (unsigned short *) malloc(sizeof(unsigned short) * size);
    return stack;
}

void destroyStack(Stack *stack){
    free(stack->data);
    free(stack);
}

void pushStack(Stack *stack, unsigned short value){
    stack->data[stack->size++] = value;
}

unsigned short popStack(Stack *stack){
    return stack->data[--stack->size];
}

bool checkInput(int verticesNumber, int edgeNumber){
    if (verticesNumber < 0 || verticesNumber > MAX_VERTICES){
        printf("bad number of vertices");
        return false;
    }else if (edgeNumber < 0 || edgeNumber > (verticesNumber * (verticesNumber + 1) / 2)) {
        printf("bad number of edges");
        return false;
    }
    return true;
}

Graph *createGraph(int verticesNumber, int edgeNumber){
    Graph *graph = (Graph *) malloc (sizeof (Graph));
    graph->verticesNumber = verticesNumber, graph->edgeNumber = edgeNumber;
    unsigned int matrixSize = ((verticesNumber + 1) * (verticesNumber + 1)) / BITS + 1;

    graph->matrix = malloc(matrixSize* sizeof (unsigned int));

    for (unsigned int i = 0; i < matrixSize; i++){
        graph->matrix[i] = 0;
    }
    return graph;
}

void destroyGraph(Graph *graph){
    free(graph->matrix);
    free(graph);
}

bool getGraph(FILE *in, Graph *graph){
    for (int i = 0; i < graph->edgeNumber; i++){
        unsigned short src, dest;
        if (fscanf(in, "%hu %hu", &src, &dest) < 2){
            printf("bad number of lines");
            return false;
        }
        if (!checkNumberOfVertices(src, dest, graph->verticesNumber)) return false;

        int num = src * graph->verticesNumber + dest;
        graph->matrix[num / BITS] |= 1 << (BITS - (num % BITS));
    }
    return true;
}

bool checkNumberOfVertices(unsigned short src, unsigned short dest, int verticesNumber){
    if (src < 1 || src > verticesNumber || dest < 1 || dest > verticesNumber){
        printf("bad vertex");
        return false;
    }
    return true;
}

bool topologicalSort(Graph *graph){
    Stack *result = createStack(graph->verticesNumber);
    unsigned char colorOfVertices[graph->verticesNumber+1];

    for (int i = 1; i <= graph->verticesNumber; i++){
        colorOfVertices[i] = 0;
    }

    for (int v = 1; v <= graph->verticesNumber; v++){
        if (colorOfVertices[v] != 2) {
            if (!dfs(graph, v, colorOfVertices, result)){
                destroyStack(result);
                return false;
            }
        }
    }

    while (result->size) printf("%d ", popStack(result));

    destroyStack(result);
    return true;
}

bool dfs(Graph *graph, unsigned short startVertex, unsigned char *colorOfVertices, Stack *result){
    colorOfVertices[startVertex] = 1;
    for (int i = 1; i <= graph->verticesNumber; i++){
        if (graph->matrix[((startVertex * graph->verticesNumber) + i) / BITS] & (1 << (BITS - (startVertex * graph->verticesNumber + i) % BITS))){
            if (colorOfVertices[i] == 1) {
                printf("impossible to sort");
                return false;
            } else if (colorOfVertices[i] != 2) {
                if (!dfs(graph, i, colorOfVertices, result)){
                    return false;
                }
            }
        }
    }
    colorOfVertices[startVertex] = 2;
    pushStack(result, startVertex);
    return true;
}
