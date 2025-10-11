#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct BstNode {
    int data;
    struct BstNode *left_child;
    struct BstNode *right_child;
} BstNode;

BstNode* createNewNode(int data);
BstNode* insertValue(BstNode* root, int data);
int performLinearSearch(const int* array, int size, int key, int* comparison_count);
BstNode* performBstSearch(BstNode* root, int key, int* comparison_count);

void releaseTreeMemory(BstNode* root) {
    if (root == NULL) return;
    releaseTreeMemory(root->left_child);
    releaseTreeMemory(root->right_child);
    free(root);
}

bool isDuplicate(const int* array, int size, int value) {
    for (int i = 0; i < size; i++) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;
}

int main() {
    srand((unsigned int)time(NULL));

    const int DATA_SIZE = 100;
    const int MAX_VALUE = 1000;

    int data_array[DATA_SIZE];
    BstNode* root_node = NULL;

    printf("\n* * * 탐색 알고리즘 성능 비교 * * *\n");
    printf(">> %d개의 고유한 난수(0-%d)를 생성합니다.\n\n", DATA_SIZE, MAX_VALUE);

    // 1. 중복되지 않는 100개의 난수를 배열과 이진 탐색 트리에 추가
    for (int i = 0; i < DATA_SIZE; ) {
        int random_val = rand() % (MAX_VALUE + 1);
        if (!isDuplicate(data_array, i, random_val)) {
            data_array[i] = random_val;
            root_node = insertValue(root_node, random_val);
            printf("%4d ", data_array[i]);
            if ((i + 1) % 10 == 0) {
                printf("\n");
            }
            i++;
        }
    }
    printf("\n");

    // 2. 사용자로부터 값을 입력받아 탐색 실행
    while (true) {
        int search_key;
        printf(" 탐색할 숫자 입력 (-1 입력 시 종료): ");
        if (scanf("%d", &search_key) != 1) {
            while(getchar() != '\n');
            printf(">> 잘못된 입력입니다. 숫자를 입력해주세요.\n");
            continue;
        }

        if (search_key == -1) {
            printf("\n 프로그램을 종료합니다.\n");
            break;
        }

        // [A] 선형 탐색 (배열)
        int linear_comparisons = 0;
        clock_t start_time_linear = clock();
        int result_index = performLinearSearch(data_array, DATA_SIZE, search_key, &linear_comparisons);
        double elapsed_time_linear = (double)(clock() - start_time_linear) / CLOCKS_PER_SEC;

        // [B] 이진 탐색 (트리)
        int bst_comparisons = 0;
        clock_t start_time_bst = clock();
        BstNode* result_node = performBstSearch(root_node, search_key, &bst_comparisons);
        double elapsed_time_bst = (double)(clock() - start_time_bst) / CLOCKS_PER_SEC;

        // --- ✨ UI가 수정된 출력 부분 ✨ ---
        printf("\n--- [ %d ] 탐색 결과 ---\n", search_key);
        printf("결과: %s\n", (result_index != -1) ? " 발견" : " 없음");
        printf("+------------------+---------------+-----------------+\n");
        printf("|     탐색 방식      |    비교 횟수    |     소요 시간      |\n");
        printf("+------------------+---------------+-----------------+\n");
        printf("| 선형 탐색          | %-13d | %-15.8f |\n", linear_comparisons, elapsed_time_linear);
        printf("| 이진 탐색 트리      | %-13d | %-15.8f |\n", bst_comparisons, elapsed_time_bst);
        printf("+------------------+---------------+-----------------+\n\n");
    }

    releaseTreeMemory(root_node);

    return 0;
}

BstNode* insertValue(BstNode* root, int data) {
    if (root == NULL) {
        return createNewNode(data);
    }
    if (data < root->data) {
        root->left_child = insertValue(root->left_child, data);
    } else if (data > root->data) {
        root->right_child = insertValue(root->right_child, data);
    }
    return root;
}

BstNode* createNewNode(int data) {
    BstNode* node = (BstNode*)malloc(sizeof(BstNode));
    node->data = data;
    node->left_child = NULL;
    node->right_child = NULL;
    return node;
}

BstNode* performBstSearch(BstNode* root, int key, int* comparison_count) {
    *comparison_count = 0;
    BstNode* current_node = root;
    while (current_node != NULL) {
        (*comparison_count)++;
        if (key == current_node->data) {
            return current_node;
        }
        current_node = (key < current_node->data) ? current_node->left_child : current_node->right_child;
    }
    return NULL;
}

int performLinearSearch(const int* array, int size, int key, int* comparison_count) {
    *comparison_count = 0;
    for (int i = 0; i < size; i++) {
        (*comparison_count)++;
        if (array[i] == key) {
            return i;
        }
    }
    return -1;
}
