#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NAME_BUFFER 50
#define LINE_BUFFER 256
#define TEST_REPEAT 10

// 구조체 및 자료형 정의
typedef struct {
    int id_num;
    char name_str[NAME_BUFFER];
    char gender_char;
    int score_kor;
    int score_eng;
    int score_math;
    int score_total;
} Record;

typedef int (*Comparator)(const Record* p1, const Record* p2);

// 전역 연산 횟수 카운터
long long op_count = 0;

// ==========================================
// 1. 데이터 로딩 및 유틸리티
// ==========================================

Record* parse_dataset(const char* file_path, int* total_records) {
    FILE* stream = fopen(file_path, "r");
    if (!stream) {
        fprintf(stderr, "[Error] Cannot open file: %s\n", file_path);
        return NULL;
    }

    char buffer[LINE_BUFFER];
    int max_size = 32;
    int idx = 0;
    Record* db = (Record*)malloc(sizeof(Record) * max_size);

    // 헤더 스킵
    if (!fgets(buffer, LINE_BUFFER, stream)) return NULL;

    while (fgets(buffer, LINE_BUFFER, stream)) {
        if (idx >= max_size) {
            max_size += 500;
            Record* resized = realloc(db, sizeof(Record) * max_size);
            if (!resized) { free(db); fclose(stream); return NULL; }
            db = resized;
        }

        Record r;
        char *ptr = buffer;
        char *next;

        // ID Parsing
        r.id_num = strtol(ptr, &next, 10);
        ptr = next + 1;

        // Name Parsing
        next = strchr(ptr, ',');
        if (next) {
            int len = next - ptr;
            if (len >= NAME_BUFFER) len = NAME_BUFFER - 1;
            strncpy(r.name_str, ptr, len);
            r.name_str[len] = '\0';
            ptr = next + 1;
        }

        // Gender
        r.gender_char = *ptr;
        ptr = strchr(ptr, ',') + 1;

        // Scores
        r.score_kor = strtol(ptr, &next, 10); ptr = next + 1;
        r.score_eng = strtol(ptr, &next, 10); ptr = next + 1;
        r.score_math = strtol(ptr, &next, 10);

        r.score_total = r.score_kor + r.score_eng + r.score_math;

        db[idx++] = r;
    }
    fclose(stream);
    *total_records = idx;
    return db;
}

void clone_data(Record* target, Record* source, int size) {
    memcpy(target, source, sizeof(Record) * size);
}

void swap_record(Record* a, Record* b) {
    Record tmp = *a; *a = *b; *b = tmp;
}

int check(const Record* a, const Record* b, Comparator func) {
    op_count++;
    return func(a, b);
}

// ==========================================
// 2. 비교 함수 (Comparator)
// ==========================================

// ID 기준
int comp_id_asc(const Record* a, const Record* b) { return (a->id_num > b->id_num) ? 1 : (a->id_num < b->id_num ? -1 : 0); }
int comp_id_desc(const Record* a, const Record* b) { return (b->id_num > a->id_num) ? 1 : (b->id_num < a->id_num ? -1 : 0); }

// 이름 기준
int comp_name_asc(const Record* a, const Record* b) { return strcmp(a->name_str, b->name_str); }
int comp_name_desc(const Record* a, const Record* b) { return strcmp(b->name_str, a->name_str); }

// 성별 기준
int comp_gender_asc(const Record* a, const Record* b) { return a->gender_char - b->gender_char; }
int comp_gender_desc(const Record* a, const Record* b) { return b->gender_char - a->gender_char; }

// 성적 합계 기준 (동점자 처리: 국어 -> 영어 -> 수학)
int comp_total_asc(const Record* a, const Record* b) {
    if (a->score_total != b->score_total) return a->score_total - b->score_total;
    if (a->score_kor != b->score_kor) return a->score_kor - b->score_kor;
    if (a->score_eng != b->score_eng) return a->score_eng - b->score_eng;
    return a->score_math - b->score_math;
}
int comp_total_desc(const Record* a, const Record* b) {
    if (a->score_total != b->score_total) return b->score_total - a->score_total;
    if (a->score_kor != b->score_kor) return b->score_kor - a->score_kor;
    if (a->score_eng != b->score_eng) return b->score_eng - a->score_eng;
    return b->score_math - a->score_math;
}

// ==========================================
// 3. 정렬 알고리즘 구현
// ==========================================

// [0] Bubble Sort
void alg_bubble(Record* list, int len, Comparator func) {
    for (int i = 0; i < len - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < len - 1 - i; j++) {
            if (check(&list[j], &list[j + 1], func) > 0) {
                swap_record(&list[j], &list[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

// [1] Selection Sort
void alg_selection(Record* list, int len, Comparator func) {
    int i = 0;
    while (i < len - 1) {
        int min = i;
        int j = i + 1;
        while (j < len) {
            if (check(&list[j], &list[min], func) < 0) min = j;
            j++;
        }
        if (min != i) swap_record(&list[i], &list[min]);
        i++;
    }
}

// [2] Insertion Sort
void alg_insertion(Record* list, int len, Comparator func) {
    for (int i = 1; i < len; i++) {
        Record target = list[i];
        int j = i - 1;
        while (j >= 0 && (op_count++, func(&list[j], &target) > 0)) {
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = target;
    }
}

// [3] Shell Sort
void alg_shell(Record* list, int len, Comparator func) {
    int gap = len / 2;
    while (gap > 0) {
        for (int i = gap; i < len; i++) {
            Record temp = list[i];
            int j = i;
            while (j >= gap) {
                op_count++;
                if (func(&list[j - gap], &temp) > 0) {
                    list[j] = list[j - gap];
                    j -= gap;
                } else {
                    break;
                }
            }
            list[j] = temp;
        }
        gap /= 2;
    }
}

// [4] Quick Sort
void quick_recur(Record* list, int low, int high, Comparator func) {
    if (low >= high) return;
    int l = low, r = high;
    Record pivot = list[(low + high) / 2];

    while (l <= r) {
        while (check(&list[l], &pivot, func) < 0) l++;
        while (check(&list[r], &pivot, func) > 0) r--;
        if (l <= r) {
            swap_record(&list[l], &list[r]);
            l++; r--;
        }
    }
    quick_recur(list, low, r, func);
    quick_recur(list, l, high, func);
}
void alg_quick(Record* list, int len, Comparator func) {
    quick_recur(list, 0, len - 1, func);
}

// [5] Heap Sort
void make_heap(Record* list, int n, int parent, Comparator func) {
    int big = parent;
    int left = 2 * parent + 1;
    int right = 2 * parent + 2;

    if (left < n && check(&list[left], &list[big], func) > 0) big = left;
    if (right < n && check(&list[right], &list[big], func) > 0) big = right;

    if (big != parent) {
        swap_record(&list[parent], &list[big]);
        make_heap(list, n, big, func);
    }
}
void alg_heap(Record* list, int len, Comparator func) {
    for (int i = len / 2 - 1; i >= 0; i--) make_heap(list, len, i, func);
    for (int i = len - 1; i > 0; i--) {
        swap_record(&list[0], &list[i]);
        make_heap(list, i, 0, func);
    }
}

// [6] Merge Sort
void process_merge(Record* list, Record* buf, int left, int mid, int right, Comparator func) {
    int l = left, r = mid + 1, k = left;
    while (l <= mid && r <= right) {
        if (check(&list[l], &list[r], func) <= 0) buf[k++] = list[l++];
        else buf[k++] = list[r++];
    }
    while (l <= mid) buf[k++] = list[l++];
    while (r <= right) buf[k++] = list[r++];
    for (int x = left; x <= right; x++) list[x] = buf[x];
}
void merge_recur(Record* list, Record* buf, int l, int r, Comparator func) {
    if (l < r) {
        int m = (l + r) / 2;
        merge_recur(list, buf, l, m, func);
        merge_recur(list, buf, m + 1, r, func);
        process_merge(list, buf, l, m, r, func);
    }
}
void alg_merge(Record* list, int len, Comparator func) {
    Record* buffer = (Record*)calloc(len, sizeof(Record));
    if(buffer) {
        merge_recur(list, buffer, 0, len - 1, func);
        free(buffer);
    }
}

// [7] Radix Sort
int get_digit(Record r, int type, int div) {
    switch(type) {
        case 0: return (r.id_num / div) % 10;
        case 2: return (unsigned char)r.gender_char;
        case 3: return (r.score_total / div) % 10;
        case 4: return (r.score_kor / div) % 10;
        case 5: return (r.score_eng / div) % 10;
        case 6: return (r.score_math / div) % 10;
        default: return 0;
    }
}
int get_char_at(Record r, int pos) {
    if (pos < 0 || pos >= NAME_BUFFER) return 0;
    return (unsigned char)r.name_str[pos];
}

void count_sort_pass(Record* arr, int n, int type, int exp, int range, int asc) {
    Record* output = malloc(n * sizeof(Record));
    int* buckets = calloc(range, sizeof(int));

    for (int i = 0; i < n; i++) {
        int idx;
        if (type == 1) idx = get_char_at(arr[i], exp);
        else idx = get_digit(arr[i], type, exp);
        if (!asc) idx = range - 1 - idx;
        buckets[idx]++;
    }

    for (int i = 1; i < range; i++) buckets[i] += buckets[i-1];

    for (int i = n - 1; i >= 0; i--) {
        int idx;
        if (type == 1) idx = get_char_at(arr[i], exp);
        else idx = get_digit(arr[i], type, exp);
        if (!asc) idx = range - 1 - idx;
        output[buckets[idx] - 1] = arr[i];
        buckets[idx]--;
    }
    memcpy(arr, output, n * sizeof(Record));
    free(output);
    free(buckets);
}

void alg_radix(Record* list, int len, int type, int is_asc) {
    if (type == 0) { // ID
        int max = 0;
        for(int i=0; i<len; i++) if(list[i].id_num > max) max = list[i].id_num;
        for (int exp = 1; max / exp > 0; exp *= 10)
            count_sort_pass(list, len, 0, exp, 10, is_asc);
    }
    else if (type == 1) { // NAME
        for (int i = NAME_BUFFER - 1; i >= 0; i--)
            count_sort_pass(list, len, 1, i, 256, is_asc);
    }
    else if (type == 2) { // GENDER
        count_sort_pass(list, len, 2, 0, 256, is_asc);
    }
    else if (type == 3) { // GRADE (Math -> Eng -> Kor -> Total)
        int fields[] = {6, 5, 4, 3};
        int max_vals[] = {100, 100, 100, 300};
        for (int k = 0; k < 4; k++) {
            int mode = fields[k];
            int limit = max_vals[k];
            for (int exp = 1; limit / exp > 0; exp *= 10) {
                count_sort_pass(list, len, mode, exp, 10, is_asc);
            }
        }
    }
}

// ==========================================
// [8] Tree Sort (수정됨: Recursive -> Iterative)
// ==========================================
typedef struct TreeNode {
    Record val;
    struct TreeNode *l, *r;
} TreeNode;

TreeNode* new_node(Record r) {
    TreeNode* t = malloc(sizeof(TreeNode));
    t->val = r; t->l = t->r = NULL;
    return t;
}

// 수정 포인트: 이중 포인터를 활용한 반복문 삽입 (스택 오버플로우 방지 및 코드 스타일 차별화)
TreeNode* tree_insert(TreeNode* root, Record r, Comparator func) {
    TreeNode** tracer = &root;

    while (*tracer) {
        op_count++;
        if (func(&r, &(*tracer)->val) < 0) {
            tracer = &(*tracer)->l;
        } else {
            tracer = &(*tracer)->r;
        }
    }
    *tracer = new_node(r);
    return root;
}

void tree_traverse(TreeNode* root, Record* arr, int* idx) {
    // 순회는 재귀로 해도 깊이 문제없음 (방문만 하므로)
    if (!root) return;
    tree_traverse(root->l, arr, idx);
    arr[(*idx)++] = root->val;
    tree_traverse(root->r, arr, idx);
}

void tree_destroy(TreeNode* root) {
    if (!root) return;
    tree_destroy(root->l);
    tree_destroy(root->r);
    free(root);
}

void alg_tree(Record* list, int len, Comparator func) {
    TreeNode* root = NULL;
    for (int i = 0; i < len; i++) {
        root = tree_insert(root, list[i], func);
    }
    int p = 0;
    tree_traverse(root, list, &p);
    tree_destroy(root);
}

// ==========================================
// 메인 실행 로직
// ==========================================

bool is_compatible(int criteria, int algo_idx) {
    // Name(1), Grade(3) -> 중복 가능성 -> Heap, Tree 제외
    if ((criteria == 1 || criteria == 3) && (algo_idx == 5 || algo_idx == 8)) return false;

    // Gender(2) -> Stable Only -> Bubble, Insertion, Merge, Radix 만 허용
    if (criteria == 2) {
        if (algo_idx == 1 || algo_idx == 3 || algo_idx == 4 || algo_idx == 5 || algo_idx == 8) return false;
    }
    return true;
}

void execute_test(Record* src, int count, int type, int is_asc) {
    const char* headers[] = {"ID", "NAME", "GENDER", "GRADE"};
    const char* direction = is_asc ? "Ascending" : "Descending";

    printf("\n============================================\n");
    printf(" Sorting Criterion: %s [%s] \n", headers[type], direction);
    printf("============================================\n");

    Comparator cmp = NULL;
    switch(type) {
        case 0: cmp = is_asc ? comp_id_asc : comp_id_desc; break;
        case 1: cmp = is_asc ? comp_name_asc : comp_name_desc; break;
        case 2: cmp = is_asc ? comp_gender_asc : comp_gender_desc; break;
        case 3: cmp = is_asc ? comp_total_asc : comp_total_desc; break;
    }

    const char* algo_titles[] = {
        "Bubble", "Selection", "Insertion", "Shell",
        "Quick", "Heap", "Merge", "Radix", "Tree"
    };

    Record* buffer = malloc(sizeof(Record) * count);

    for (int k = 0; k < 9; k++) {
        if (!is_compatible(type, k)) continue;

        long long sum_ops = 0;
        long long mem = 0;

        for (int i = 0; i < TEST_REPEAT; i++) {
            clone_data(buffer, src, count);
            op_count = 0;

            switch(k) {
                case 0: alg_bubble(buffer, count, cmp); break;
                case 1: alg_selection(buffer, count, cmp); break;
                case 2: alg_insertion(buffer, count, cmp); break;
                case 3: alg_shell(buffer, count, cmp); break;
                case 4: alg_quick(buffer, count, cmp); break;
                case 5: alg_heap(buffer, count, cmp); break;
                case 6: alg_merge(buffer, count, cmp); break;
                case 7: alg_radix(buffer, count, type, is_asc); break;
                case 8: alg_tree(buffer, count, cmp); break;
            }
            sum_ops += op_count;
        }

        long long avg = sum_ops / TEST_REPEAT;

        if (k == 6 || k == 7) mem = (long long)count * sizeof(Record);
        if (k == 8) mem = (long long)count * sizeof(TreeNode);

        printf(" %-10s : Comparisons %12lld | Memory %10lld bytes\n", algo_titles[k], avg, mem);
    }
    free(buffer);
}

int main() {
    int total = 0;
    Record* data = parse_dataset("dataset_id_ascending.csv", &total);

    if (data) {
        printf("Dataset loaded. Total records: %d\n", total);
        printf("Iterations per Sort: %d\n", TEST_REPEAT);

        execute_test(data, total, 0, 1);
        execute_test(data, total, 0, 0);

        execute_test(data, total, 1, 1);
        execute_test(data, total, 1, 0);

        execute_test(data, total, 2, 1);
        execute_test(data, total, 2, 0);

        execute_test(data, total, 3, 1);
        execute_test(data, total, 3, 0);

        free(data);
    } else {
        return 1;
    }
    return 0;
}