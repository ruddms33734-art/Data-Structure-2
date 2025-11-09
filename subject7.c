#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- 상수 정의 ---
#define V 10   // 정점의 갯수
#define E 20   // 간선의 갯수
#define INF 99999

#define TOTAL_POSSIBLE_EDGES 45

typedef struct {
    int u;
    int v;
} Edge;

// --- 함수 프로토타입 선언 ---
void init_graph(int dist[V][V], int via[V][V]);
void add_random_edges(int dist[V][V], Edge edges[]);
void shuffle_edges(Edge edges[], int n);
void swap_edges(Edge *a, Edge *b);
void floyd_warshall(int dist[V][V], int via[V][V]);
void print_all_paths(int dist[V][V], int via[V][V]);
void print_recursive_path(int i, int j, int via[V][V]);


// --- 메인 함수 ---
int main() {
    int dist[V][V];
    int via[V][V];

    Edge possible_edges[TOTAL_POSSIBLE_EDGES];

    srand(time(NULL));

    init_graph(dist, via);

    add_random_edges(dist, possible_edges);

    floyd_warshall(dist, via);

    print_all_paths(dist, via);

    return 0;
}

/*
    1. 그래프(거리 행렬) 초기화 함수
*/
void init_graph(int dist[V][V], int via[V][V]) {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) {
                dist[i][j] = 0;
            } else {
                dist[i][j] = INF;
            }
            via[i][j] = -1;
        }
    }
}

void add_random_edges(int dist[V][V], Edge edges[]) {
    int count = 0;
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            edges[count].u = i;
            edges[count].v = j;
            count++;
        }
    }

    printf("--- %d개의 모든 간선 리스트를 섞는 중... ---\n", TOTAL_POSSIBLE_EDGES);
    shuffle_edges(edges, TOTAL_POSSIBLE_EDGES);

    printf("--- 셔플된 리스트에서 %d개 간선 추가 중... ---\n", E);
    for (int i = 0; i < E; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        dist[u][v] = 1;
        dist[v][u] = 1;
    }
    printf("--- 간선 생성 완료 ---\n\n");
}

void shuffle_edges(Edge edges[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap_edges(&edges[i], &edges[j]);
    }
}

void swap_edges(Edge *a, Edge *b) {
    Edge temp = *a;
    *a = *b;
    *b = temp;
}


/*
    3. 플로이드-워셜 알고리즘 실행 함수
*/
void floyd_warshall(int dist[V][V], int via[V][V]) {

    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {

                int new_dist;
                if (dist[i][k] == INF || dist[k][j] == INF) {
                    new_dist = INF;
                } else {
                    new_dist = dist[i][k] + dist[k][j];
                }

                if (dist[i][j] > new_dist) {
                    dist[i][j] = new_dist;
                    via[i][j] = k;
                }
            }
        }
    }
}

/*
    4. 모든 쌍(45쌍) 최단 경로 및 경로 출력 함수
*/
void print_all_paths(int dist[V][V], int via[V][V]) {
    printf("--- 모든 노드 쌍(45쌍) 최단 거리 및 경로 결과 ---\n");
    int pair_count = 0;

    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            pair_count++;
            printf("  (%2d, %2d) 쌍: ", i, j);

            if (dist[i][j] == INF) {
                printf("∞ (경로 없음)\n");
            } else {
                printf("거리 %d [경로: ", dist[i][j]);

                printf("%d -> ", i);

                print_recursive_path(i, j, via);

                printf("%d]\n", j);
            }
        }
    }
    printf("\n총 %d쌍의 최단 경로를 출력했습니다.\n", pair_count);
}

void print_recursive_path(int i, int j, int via[V][V]) {
    int k = via[i][j];

    if (k == -1) {
        return;
    }

    print_recursive_path(i, k, via);

    printf("%d -> ", k);

    print_recursive_path(k, j, via);
}