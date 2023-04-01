#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#define ERROR_INPUT 0
#define CORRECT_INPUT 1


typedef struct Subset {int parent;} Subset;
typedef unsigned long long ull;
typedef struct Edge {int src, dest, weight;} Edge;
typedef struct Graph {int verticesNumber, edgeNumber; Edge *edges;} Graph;

int compare(const void *e1, const void *e2);
Graph *createGraph(int N, int M);
int getGraph(Graph *graph, FILE *in, int edges_number, int vertices_number);
int checkEdge(int src, int dest, ull weight, int vertices_number);
Subset *createDSU(Graph *graph);
int find(Subset *subsets, int i);
void Union(Subset *subsets, int x, int y);
void kruskal(Graph *graph);


int main() {
    int M, N;
    FILE *in = fopen("in.txt", "r");
    if (fscanf(in, "%d", &N)) {};
    if (fscanf(in, "%d", &M)) {};

    if (N < 0 || N > 5000) {
        fclose(in);
        printf("bad number of vertices");
        return 0;
    }
    if (M < 0 || M > (N * (N + 1) / 2)) {
        fclose(in);
        printf("bad number of edges");
        return 0;
    }

    Graph *graph = createGraph(N, M);
    if (!getGraph(graph, in, M, N)) {
        free(graph->edges);
        free(graph);
        fclose(in);
        return 0;
    }

    kruskal(graph);
    fclose(in);
    free(graph->edges);
    free(graph);
    return 0;
}

Graph *createGraph(int N, int M) {
    Graph *graph = (Graph *) malloc(sizeof(Graph));
    graph->verticesNumber = N;
    graph->edgeNumber = M;
    graph->edges = (Edge *) malloc(M * sizeof(Edge));
    return graph;
}

int getGraph(Graph *graph, FILE *in, int edges_number, int vertices_number) {
    for (int edge = 0; edge < edges_number; edge++) {
        int src, dest;
        ull weight;
        if (fscanf(in, "%d %d %llu", &src, &dest, &weight) < 3) {
            printf("bad number of lines");
            return ERROR_INPUT;
        };
        if (!checkEdge(src, dest, weight, vertices_number)) return ERROR_INPUT;

        graph->edges[edge].src = src;
        graph->edges[edge].dest = dest;
        graph->edges[edge].weight = (int) weight;

    }
    return CORRECT_INPUT;
}

int checkEdge(int src, int dest, ull weight, int vertices_number) {
    if ((src < 1 || src > vertices_number) || (dest < 1 || dest > vertices_number)) {
        printf("bad vertex");
        return ERROR_INPUT;
    }
    if (weight > INT_MAX) {
        printf("bad length");
        return ERROR_INPUT;
    }
    return CORRECT_INPUT;
}

int compare(const void *e1, const void *e2) {
    Edge *edge1 = (Edge *) e1;
    Edge *edge2 = (Edge *) e2;
    return edge1->weight > edge2->weight;
}

Subset *createDSU(Graph *graph) {
    Subset *subsets = (Subset *) malloc(sizeof(Subset) * (graph->verticesNumber + 1));
    for (int v = 1; v <= graph->verticesNumber; v++) {
        subsets[v].parent = v;
    }
    return subsets;
}

int find(Subset *subsets, int i) {
    if (subsets[i].parent != i) {
        subsets[i].parent = find(subsets, subsets[i].parent);
    }
    return subsets[i].parent;
}

void Union(Subset *subsets, int x, int y) {
    int xRoot = find(subsets, x);
    int yRoot = find(subsets, y);

    if (rand() % 2 != 0) {
        subsets[xRoot].parent = yRoot;
    } else {
        subsets[yRoot].parent = xRoot;
    }
}

void kruskal(Graph *graph) {
    int edgesCount = 0;
    int resultIndex = 0;
    int countOfUnrelatedVertices = graph->verticesNumber - 1;
    Edge *result = malloc(sizeof(Edge) * graph->edgeNumber);

    qsort(graph->edges, graph->edgeNumber, sizeof(Edge), compare);

    Subset *subsets = createDSU(graph);
    while (edgesCount < graph->edgeNumber) {
        Edge current_edge = graph->edges[edgesCount++];

        int x = find(subsets, current_edge.src);
        int y = find(subsets, current_edge.dest);

        if (x != y) {
            countOfUnrelatedVertices--;
            result[resultIndex++] = current_edge;
            Union(subsets, x, y);
        }
    }
    if (countOfUnrelatedVertices != 0) {
        free(subsets);
        free(result);
        printf("no spanning tree");
        return;
    }

    for (int i = 0; i < resultIndex; i++) {
        printf("%d %d\n", result[i].src, result[i].dest);
    }
    free(subsets);
    free(result);
}
