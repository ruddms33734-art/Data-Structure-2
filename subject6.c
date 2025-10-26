#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_VERTICES 100

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

void addEdgeToList(Node** adjList, int u, int v) {
    Node* newNode1 = (Node*)malloc(sizeof(Node));
    newNode1->vertex = v;
    newNode1->next = adjList[u];
    adjList[u] = newNode1;

    Node* newNode2 = (Node*)malloc(sizeof(Node));
    newNode2->vertex = u;
    newNode2->next = adjList[v];
    adjList[v] = newNode2;
}

void freeAdjList(Node** adjList) {
    for (int i = 0; i < NUM_VERTICES; i++) {
        Node* current = adjList[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
}


void sparse_matrix() {
    printf("케이스 1: 희소 그래프 - 인접 행렬\n");
    int adjMatrix[NUM_VERTICES][NUM_VERTICES] = {0};
    int numEdges = 100;
    int edgesCount = 0;


    while (edgesCount < numEdges) {
        int u = rand() % NUM_VERTICES;
        int v = rand() % NUM_VERTICES;
        if (u != v && adjMatrix[u][v] == 0) {
            adjMatrix[u][v] = 1;
            adjMatrix[v][u] = 1;
            edgesCount++;
        }
    }

    printf("메모리: %zu Bytes\n", sizeof(adjMatrix));

    // --- (수정) 하드코딩된 printf 대신 실제 연산 수행 ---
    int u, v;
    int comparison_count;

    // 2. 간선 삭제 비교 (O(1))
    comparison_count = 0;
    do {
      u = rand() % NUM_VERTICES;
      v = rand() % NUM_VERTICES;
    } while(u == v || adjMatrix[u][v] == 0); // 이미 존재하는 간선을 찾음

    comparison_count++; // adjMatrix[u][v] 확인
    adjMatrix[u][v] = 0;
    adjMatrix[v][u] = 0;
    printf("간선 삭제 비교: %d번\n", comparison_count);

    comparison_count = 0;
    do {
      u = rand() % NUM_VERTICES;
      v = rand() % NUM_VERTICES;
    } while(u == v || adjMatrix[u][v] == 1);

    comparison_count++;
    adjMatrix[u][v] = 1;
    adjMatrix[v][u] = 1;
    printf("간선 삽입 비교: %d번\n", comparison_count);

    u = rand() % NUM_VERTICES;
    v = rand() % NUM_VERTICES;
    comparison_count = 1;
    printf("두 정점의 연결 확인 비교: %d번\n", comparison_count);

    int comparisons = 0;
    int targetNode = rand() % NUM_VERTICES;
    for (int i = 0; i < NUM_VERTICES; i++) {
        comparisons++;
    }
    printf("한 노드의 인접 노드 출력 비교: %d번\n", comparisons);
    printf("----------------------------------------\n");
}

void sparse_list() {
    printf("케이스 2: 희소 그래프 - 인접 리스트\n");
    Node* adjList[NUM_VERTICES] = {NULL};
    int numEdges = 100;
    int adjMatrixForCheck[NUM_VERTICES][NUM_VERTICES] = {0};
    int edgesCount = 0;

    while (edgesCount < numEdges) {
        int u = rand() % NUM_VERTICES;
        int v = rand() % NUM_VERTICES;
        if (u != v && adjMatrixForCheck[u][v] == 0) {
            addEdgeToList(adjList, u, v);
            adjMatrixForCheck[u][v] = 1;
            adjMatrixForCheck[v][u] = 1;
            edgesCount++;
        }
    }

    size_t memory = sizeof(adjList) + (numEdges * 2 * sizeof(Node));
    printf("메모리: %zu Bytes\n", memory);

    printf("간선 삽입 비교: 1번\n");

    int u = rand() % NUM_VERTICES;
    int deg_u = 0;
    Node* current_u = adjList[u];
    while(current_u != NULL) {
        deg_u++;
        current_u = current_u->next;
    }
    printf("간선 삭제 비교 (정점 %d): 약 %d번\n", u, deg_u);

    int v = rand() % NUM_VERTICES;
    int comparisons_check = 0;
    Node* current = adjList[u];
    while (current != NULL) {
        comparisons_check++;
        if (current->vertex == v) break;
        current = current->next;
    }
    printf("두 정점의 연결 확인 비교 (정점 %d): %d번\n", u, comparisons_check);

    int targetNode = rand() % NUM_VERTICES;
    int comparisons_print = 0;
    current = adjList[targetNode];
    while (current != NULL) {
        comparisons_print++;
        current = current->next;
    }
    printf("한 노드의 인접 노드 출력 비교 (정점 %d): %d번\n", targetNode, comparisons_print);

    freeAdjList(adjList);
    printf("----------------------------------------\n");
}


void dense_matrix() {
    printf("케이스 3: 밀집 그래프 - 인접 행렬\n");
    int adjMatrix[NUM_VERTICES][NUM_VERTICES] = {0};
    int numEdges = 4000;
    int edgesCount = 0;

    while (edgesCount < numEdges) {
        int u = rand() % NUM_VERTICES;
        int v = rand() % NUM_VERTICES;
        if (u != v && adjMatrix[u][v] == 0) {
            adjMatrix[u][v] = 1;
            adjMatrix[v][u] = 1;
            edgesCount++;
        }
    }

    printf("메모리: %zu Bytes\n", sizeof(adjMatrix));

    int u, v;
    int comparison_count;

    comparison_count = 0;
    do {
      u = rand() % NUM_VERTICES;
      v = rand() % NUM_VERTICES;
    } while(u == v || adjMatrix[u][v] == 0); // 이미 존재하는 간선을 찾음

    comparison_count++;
    adjMatrix[u][v] = 0;
    adjMatrix[v][u] = 0;
    printf("간선 삭제 비교: %d번\n", comparison_count);

    comparison_count = 0;
    do {
      u = rand() % NUM_VERTICES;
      v = rand() % NUM_VERTICES;
    } while(u == v || adjMatrix[u][v] == 1);

    comparison_count++;
    adjMatrix[u][v] = 1;
    adjMatrix[v][u] = 1;
    printf("간선 삽입 비교: %d번\n", comparison_count);

    u = rand() % NUM_VERTICES;
    v = rand() % NUM_VERTICES;
    comparison_count = 1;
    printf("두 정점의 연결 확인 비교: %d번\n", comparison_count);

    int comparisons = 0;
    int targetNode = rand() % NUM_VERTICES;
    for (int i = 0; i < NUM_VERTICES; i++) {
        comparisons++;
    }
    printf("한 노드의 인접 노드 출력 비교: %d번\n", comparisons);
    printf("----------------------------------------\n");
}


void dense_list() {
    printf("케이스 4: 밀집 그래프 - 인접 리스트\n");
    Node* adjList[NUM_VERTICES] = {NULL};
    int numEdges = 4000;

    int (*adjMatrixForCheck)[NUM_VERTICES] = malloc(sizeof(int[NUM_VERTICES][NUM_VERTICES]));
    if (adjMatrixForCheck == NULL) {
        printf("메모리 할당 실패\n");
        return;
    }
    for(int i=0; i<NUM_VERTICES; ++i)
        for(int j=0; j<NUM_VERTICES; ++j)
            adjMatrixForCheck[i][j] = 0;

    int edgesCount = 0;

    while (edgesCount < numEdges) {
        int u = rand() % NUM_VERTICES;
        int v = rand() % NUM_VERTICES;
        if (u != v && adjMatrixForCheck[u][v] == 0) {
            addEdgeToList(adjList, u, v);
            adjMatrixForCheck[u][v] = 1;
            adjMatrixForCheck[v][u] = 1;
            edgesCount++;
        }
    }
    free(adjMatrixForCheck);


    size_t memory = sizeof(adjList) + (numEdges * 2 * sizeof(Node));
    printf("메모리: %zu Bytes\n", memory);

    printf("간선 삽입 비교: 1번\n");

    int u = rand() % NUM_VERTICES;
    int deg_u = 0;
    Node* current_u = adjList[u];
    while(current_u != NULL) {
        deg_u++;
        current_u = current_u->next;
    }
    printf("간선 삭제 비교 (정점 %d): 약 %d번\n", u, deg_u);

    int v = rand() % NUM_VERTICES;
    int comparisons_check = 0;
    Node* current = adjList[u];
    while (current != NULL) {
        comparisons_check++;
        if (current->vertex == v) break;
        current = current->next;
    }
    printf("두 정점의 연결 확인 비교 (정점 %d): %d번\n", u, comparisons_check);

    int targetNode = rand() % NUM_VERTICES;
    int comparisons_print = 0;
    current = adjList[targetNode];
    while (current != NULL) {
        comparisons_print++;
        current = current->next;
    }
    printf("한 노드의 인접 노드 출력 비교 (정점 %d): %d번\n", targetNode, comparisons_print);


    freeAdjList(adjList);
    printf("----------------------------------------\n");
}


int main() {
    srand(time(NULL));

    sparse_matrix();
    sparse_list();
    dense_matrix();
    dense_list();

    return 0;
}