#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_RECORDS 35000  // 데이터셋 크기에 맞춰 넉넉하게 설정

// 학생 구조체 (데이터)
typedef struct {
    int id;
    char name[50];
    char gender[10];
    int korean;
    int english;
    int math;
} Student;

// AVL 트리 노드
typedef struct Node {
    Student data;
    struct Node *left;
    struct Node *right;
    int height;
} Node;

// 전역 변수: 비교 횟수 카운팅
long long comparison_count = 0;

// 유틸리티 함수: 높이 구하기
int getHeight(Node *N) {
    if (N == NULL) return 0;
    return N->height;
}

// 유틸리티 함수: 최댓값
int max(int a, int b) {
    return (a > b) ? a : b;
}

// 새 노드 생성
Node* newNode(Student data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// 오른쪽 회전
Node *rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

// 왼쪽 회전
Node *leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

// 균형 계수 구하기
int getBalance(Node *N) {
    if (N == NULL) return 0;
    return getHeight(N->left) - getHeight(N->right);
}

// AVL 삽입 함수
Node* insert(Node* node, Student data) {
    // 1. 일반적인 BST 삽입
    if (node == NULL)
        return newNode(data);

    comparison_count++; // 비교 발생!
    if (data.id < node->data.id)
        node->left = insert(node->left, data);
    else if (data.id > node->data.id)
        node->right = insert(node->right, data);
    else
        return node; // 중복 ID

    // 2. 높이 업데이트
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // 3. 균형 계수 확인 및 회전
    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && data.id < node->left->data.id)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && data.id > node->right->data.id)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && data.id > node->left->data.id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && data.id < node->right->data.id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// *** 핵심 로직: 최적화된 순서로 삽입 (재귀적 분할) ***
void insertOptimizedOrder(Node** root, Student* arr, int start, int end) {
    if (start > end) return;

    // 중간 인덱스 계산
    int mid = (start + end) / 2;

    // 중간값을 트리에 삽입
    *root = insert(*root, arr[mid]);

    // 왼쪽 절반 처리 (재귀)
    insertOptimizedOrder(root, arr, start, mid - 1);

    // 오른쪽 절반 처리 (재귀)
    insertOptimizedOrder(root, arr, mid + 1, end);
}

// 메모리 해제 (후위 순회)
void freeTree(Node* node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

int main() {
    FILE *fp = fopen("dataset_id_ascending.csv", "r");
    if (!fp) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }

    // 데이터 로드용 배열
    static Student students[MAX_RECORDS];
    char line[MAX_LINE_LENGTH];
    int count = 0;

    // 헤더 건너뛰기
    fgets(line, MAX_LINE_LENGTH, fp);

    // CSV 파싱
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        Student s;
        char *token = strtok(line, ","); s.id = atoi(token);
        token = strtok(NULL, ","); strcpy(s.name, token);
        token = strtok(NULL, ","); strcpy(s.gender, token);
        token = strtok(NULL, ","); s.korean = atoi(token);
        token = strtok(NULL, ","); s.english = atoi(token);
        token = strtok(NULL, ","); s.math = atoi(token);

        students[count++] = s;
    }
    fclose(fp);

    printf("데이터 로드 완료: %d건\n", count);
    printf("최적화 정렬(AVL 삽입) - 10회 반복 평균 측정 중...\n");
    printf("----------------------------------------\n");

    long long total_comparisons = 0;
    int runs = 10;

    for (int i = 0; i < runs; i++) {
        // [중요] 매 반복마다 초기화
        Node *root = NULL;
        comparison_count = 0;

        // 최적화된 삽입 실행
        insertOptimizedOrder(&root, students, 0, count - 1);

        // 결과 누적
        total_comparisons += comparison_count;

        // [중요] 다음 반복을 위해 트리 메모리 해제
        freeTree(root);
    }

    double average_comparisons = (double)total_comparisons / runs;

    // 결과 출력
    printf("총 반복 횟수: %d회\n", runs);
    printf("평균 비교 횟수: %.1f\n", average_comparisons);
    printf("----------------------------------------\n");

    return 0;
}