#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ========== 1. 상수 및 전역 변수 정의 ==========

#define SIZE 1000     // 데이터 개수
#define MAX_VAL 10001 // 난수 범위 (0 ~ 10000)

// 탐색 횟수를 기록하기 위한 전역 변수
// (함수 파라미터로 넘기는 것보다 구현이 간편하여 사용)
long long g_comparison_count = 0;

// ========== 2. 트리 노드 구조체 정의 ==========

typedef struct Node {
    int key;
    struct Node *left;
    struct Node *right;
    int height; // AVL 트리의 균형을 위해 높이 정보 추가
} Node;

// ========== 3. 유틸리티 함수 (공통 사용) ==========

// 두 정수 중 큰 값을 반환
int max(int a, int b) {
    return (a > b) ? a : b;
}

// 노드의 높이를 반환 (NULL이면 0)
int height(Node *N) {
    if (N == NULL)
        return 0;
    return N->height;
}

// 새 노드 생성 유틸리티
Node* newNode(int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // 새 노드는 항상 높이 1
    return node;
}

// 배열 셔플 (데이터셋 1 생성용)
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// ========== 4. AVL 트리 핵심 함수 ==========

// 오른쪽 회전 (LL Case)
Node *rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    // 회전 수행
    x->right = y;
    y->left = T2;

    // 높이 갱신
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // 새로운 루트 반환
    return x;
}

// 왼쪽 회전 (RR Case)
Node *leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    // 회전 수행
    y->left = x;
    x->right = T2;

    // 높이 갱신
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // 새로운 루트 반환
    return y;
}

// 노드의 균형 인수(Balance Factor) 반환
int getBalance(Node *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// AVL 트리 삽입 함수
Node* avl_insert(Node* node, int key) {
    // 1. 표준 BST 삽입 수행
    if (node == NULL)
        return(newNode(key));

    if (key < node->key)
        node->left = avl_insert(node->left, key);
    else if (key > node->key)
        node->right = avl_insert(node->right, key);
    else // 중복 키는 허용하지 않음 (과제 조건에 따라)
        return node;

    // 2. 노드 높이 갱신
    node->height = 1 + max(height(node->left), height(node->right));

    // 3. 균형 인수(BF) 확인
    int balance = getBalance(node);

    // 4. 불균형 상태라면 4가지 케이스로 나누어 회전 수행

    // LL (Left Left) Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // RR (Right Right) Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // LR (Left Right) Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // RL (Right Left) Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // 균형이 맞는 경우, 원래 노드 포인터 반환
    return node;
}

// ========== 5. BST(이진탐색트리) 함수 ==========

// BST 삽입 함수 (AVL과 달리 회전/균형잡기 없음)
Node* bst_insert(Node* node, int key) {
    if (node == NULL)
        return newNode(key); // 높이 정보는 BST에선 불필요하지만 편의상 재사용

    if (key < node->key)
        node->left = bst_insert(node->left, key);
    else if (key > node->key)
        node->right = bst_insert(node->right, key);

    return node;
}

// ========== 6. 탐색 함수 (비교 횟수 카운트) ==========

// (1) 배열: 선형 탐색
void linear_search(int arr[], int key) {
    for (int i = 0; i < SIZE; i++) {
        g_comparison_count++; // 비교 횟수 증가
        if (arr[i] == key) {
            return; // 찾음
        }
    }
    // 못 찾아도 여기까지 비교한 횟수가 기록됨
}

// (2) BST & AVL: 트리 탐색 (두 트리의 탐색 로직은 동일)
void tree_search(Node* root, int key) {
    if (root == NULL) {
        return; // 못 찾음
    }

    g_comparison_count++; // 현재 노드와 비교 횟수 증가

    if (root->key == key) {
        return; // 찾음
    } else if (key < root->key) {
        tree_search(root->left, key);
    } else {
        tree_search(root->right, key);
    }
}

// ========== 7. 데이터 생성 함수 ==========

// 데이터 (1): 0~10000 사이의 무작위 정수 1000개 (중복 X)
void create_dataset_1(int arr[]) {
    int* all_nums = (int*)malloc(sizeof(int) * MAX_VAL);
    for (int i = 0; i < MAX_VAL; i++) {
        all_nums[i] = i;
    }
    // Fisher-Yates Shuffle
    for (int i = MAX_VAL - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&all_nums[i], &all_nums[j]);
    }
    // 앞 1000개만 복사
    for (int i = 0; i < SIZE; i++) {
        arr[i] = all_nums[i];
    }
    free(all_nums);
}

// 데이터 (2): 0~999까지 정렬된 정수 1000개
void create_dataset_2(int arr[]) {
    for (int i = 0; i < SIZE; i++) {
        arr[i] = i;
    }
}

// 데이터 (3): 999~0까지 정렬된 정수 1000개
void create_dataset_3(int arr[]) {
    for (int i = 0; i < SIZE; i++) {
        arr[i] = 999 - i;
    }
}

// 데이터 (4): 공식에 따른 정수 1000개
void create_dataset_4(int arr[]) {
    for (int i = 0; i < SIZE; i++) {
        arr[i] = i * (i % 2 + 2);
    }
}

// 탐색용 키 생성: 0~10000 사이 난수 1000개 (중복 허용)
void create_search_keys(int keys[]) {
    for (int i = 0; i < SIZE; i++) {
        keys[i] = rand() % MAX_VAL;
    }
}

// ========== 8. 메모리 해제 함수 ==========

// 트리에 할당된 메모리 해제 (후위 순회 방식)
void free_tree(Node* node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

// ========== 9. 실험 진행 및 출력 함수 ==========

void run_experiment(int data[], int search_keys[], int dataset_num) {
    // 1. 자료구조 선언 및 구축
    int array_data[SIZE];
    Node* bst_root = NULL;
    Node* avl_root = NULL;

    for (int i = 0; i < SIZE; i++) {
        // (1) 배열 삽입
        array_data[i] = data[i];

        // (2) BST 삽입
        bst_root = bst_insert(bst_root, data[i]);

        // (3) AVL 삽입
        avl_root = avl_insert(avl_root, data[i]);
    }

    // 2. 총 탐색 횟수 기록용 변수
    long long array_total_comps = 0;
    long long bst_total_comps = 0;
    long long avl_total_comps = 0;

    // 3. 1000개의 탐색 키로 각각 탐색 수행
    for (int i = 0; i < SIZE; i++) {
        int key_to_find = search_keys[i];

        // (1) 배열 탐색
        g_comparison_count = 0; // 탐색 전 횟수 초기화
        linear_search(array_data, key_to_find);
        array_total_comps += g_comparison_count;

        // (2) BST 탐색
        g_comparison_count = 0; // 탐색 전 횟수 초기화
        tree_search(bst_root, key_to_find);
        bst_total_comps += g_comparison_count;

        // (3) AVL 탐색
        g_comparison_count = 0; // 탐색 전 횟수 초기화
        tree_search(avl_root, key_to_find);
        avl_total_comps += g_comparison_count;
    }

    // 4. 결과 출력
    printf("--- [데이터 (%d) 실험 결과] ---\n", dataset_num);
    printf("Array: 데이터 (%d)에서 평균 %.2f회 탐색\n", dataset_num, (double)array_total_comps / SIZE);
    printf("BST:   데이터 (%d)에서 평균 %.2f회 탐색\n", dataset_num, (double)bst_total_comps / SIZE);
    printf("AVL:   데이터 (%d)에서 평균 %.2f회 탐색\n", dataset_num, (double)avl_total_comps / SIZE);
    printf("\n");

    // 5. 메모리 해제
    free_tree(bst_root);
    free_tree(avl_root);
}

// ========== 10. 메인 함수 ==========

int main() {
    // 난수 시드 초기화
    srand((unsigned int)time(NULL));

    // 1000개의 초기 데이터(data)와 1000개의 탐색 키(keys)를 담을 배열
    int data[SIZE];
    int search_keys[SIZE];

    // 탐색용 키 1000개 미리 생성
    create_search_keys(search_keys);

    // --- 실험 1: 데이터 (1) 사용 ---
    create_dataset_1(data);
    run_experiment(data, search_keys, 1);

    // --- 실험 2: 데이터 (2) 사용 ---
    create_dataset_2(data);
    run_experiment(data, search_keys, 2);

    // --- 실험 3: 데이터 (3) 사용 ---
    create_dataset_3(data);
    run_experiment(data, search_keys, 3);

    // --- 실험 4: 데이터 (4) 사용 ---
    create_dataset_4(data);
    run_experiment(data, search_keys, 4);

    return 0;
}