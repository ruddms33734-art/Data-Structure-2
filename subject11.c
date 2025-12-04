#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_SIZE 50
#define LINE_BUF 200

typedef struct {
    int id;
    char name[NAME_SIZE];
    char gender;
    int kor;
    int eng;
    int math;
} Student;

typedef struct AVL {
    Student val;
    struct AVL *lch;
    struct AVL *rch;
    int height;
} AVL;

int max_int(int a, int b) { return (a > b) ? a : b; }
int get_height(AVL* n) { return n ? n->height : 0; }

Student* read_csv(const char* path, int* count) {
    FILE* f = fopen(path, "r");
    if (!f) { perror("파일 열기 실패"); return NULL; }

    char buf[LINE_BUF];
    int cap = 16, cnt = 0;
    Student* arr = malloc(sizeof(Student) * cap);

    fgets(buf, sizeof(buf), f); // 헤더 스킵

    while (fgets(buf, sizeof(buf), f)) {
        if (cnt >= cap) {
            cap *= 2;
            Student* tmp = realloc(arr, sizeof(Student) * cap);
            if (!tmp) { perror("realloc 실패"); free(arr); fclose(f); return NULL; }
            arr = tmp;
        }

        Student s;
        char* token = strtok(buf, ",");
        if (token) s.id = atoi(token);

        token = strtok(NULL, ",");
        if (token) strncpy(s.name, token, NAME_SIZE);

        token = strtok(NULL, ",");
        if (token) s.gender = token[0];

        token = strtok(NULL, ",");
        if (token) s.kor = atoi(token);

        token = strtok(NULL, ",");
        if (token) s.eng = atoi(token);

        token = strtok(NULL, ",");
        if (token) s.math = atoi(token);

        arr[cnt++] = s;
    }

    fclose(f);
    *count = cnt;
    return arr;
}

// ================== Unsorted Array ==================
void ua_add(Student** arr, int* cnt, int* cap, Student data) {
    if (*cnt >= *cap) {
        *cap *= 2;
        *arr = realloc(*arr, sizeof(Student) * (*cap));
    }
    (*arr)[(*cnt)++] = data;
}

int ua_find(Student* arr, int n, int id, long long* cmp) {
    for (int i = 0; i < n; i++) {
        (*cmp)++;
        if (arr[i].id == id) return i;
    }
    return -1;
}

void ua_remove(Student* arr, int* n, int id, long long* cmp) {
    int idx = ua_find(arr, *n, id, cmp);
    if (idx != -1) {
        arr[idx] = arr[--(*n)];
    }
}

// ================== Sorted Array ==================
int sa_pos(Student* arr, int n, int id, long long* cmp) {
    int l = 0, r = n - 1, res = n;
    while (l <= r) {
        int m = (l + r) / 2;
        (*cmp)++;
        if (arr[m].id >= id) {
            res = m;
            r = m - 1;
        } else l = m + 1;
    }
    return res;
}

void sa_add(Student** arr, int* n, int* cap, Student s, long long* cmp) {
    if (*n >= *cap) {
        *cap *= 2;
        *arr = realloc(*arr, sizeof(Student) * (*cap));
    }
    int idx = sa_pos(*arr, *n, s.id, cmp);
    memmove(&(*arr)[idx + 1], &(*arr)[idx], sizeof(Student) * (*n - idx));
    (*arr)[idx] = s;
    (*n)++;
}

int sa_search(Student* arr, int n, int id, long long* cmp) {
    int l = 0, r = n - 1;
    while (l <= r) {
        int m = (l + r) / 2;
        (*cmp)++;
        if (arr[m].id == id) return m;
        if (arr[m].id < id) l = m + 1;
        else r = m - 1;
    }
    return -1;
}

void sa_remove(Student* arr, int* n, int id, long long* cmp) {
    int idx = sa_search(arr, *n, id, cmp);
    if (idx != -1) {
        memmove(&arr[idx], &arr[idx + 1], sizeof(Student) * (*n - idx - 1));
        (*n)--;
    }
}

// ================== AVL Tree ==================
AVL* avl_new(Student s) {
    AVL* n = malloc(sizeof(AVL));
    n->val = s;
    n->lch = n->rch = NULL;
    n->height = 1;
    return n;
}

AVL* rotate_left(AVL* x) {
    AVL* y = x->rch;
    AVL* t = y->lch;
    y->lch = x;
    x->rch = t;
    x->height = max_int(get_height(x->lch), get_height(x->rch)) + 1;
    y->height = max_int(get_height(y->lch), get_height(y->rch)) + 1;
    return y;
}

AVL* rotate_right(AVL* y) {
    AVL* x = y->lch;
    AVL* t = x->rch;
    x->rch = y;
    y->lch = t;
    y->height = max_int(get_height(y->lch), get_height(y->rch)) + 1;
    x->height = max_int(get_height(x->lch), get_height(x->rch)) + 1;
    return x;
}

int avl_balance(AVL* n) {
    return n ? get_height(n->lch) - get_height(n->rch) : 0;
}

AVL* avl_insert(AVL* root, Student s, long long* cmp) {
    if (!root) return avl_new(s);

    (*cmp)++;
    if (s.id < root->val.id) root->lch = avl_insert(root->lch, s, cmp);
    else if (s.id > root->val.id) root->rch = avl_insert(root->rch, s, cmp);
    else return root;

    root->height = 1 + max_int(get_height(root->lch), get_height(root->rch));
    int bal = avl_balance(root);

    if (bal > 1 && s.id < root->lch->val.id) return rotate_right(root);
    if (bal < -1 && s.id > root->rch->val.id) return rotate_left(root);
    if (bal > 1 && s.id > root->lch->val.id) {
        root->lch = rotate_left(root->lch);
        return rotate_right(root);
    }
    if (bal < -1 && s.id < root->rch->val.id) {
        root->rch = rotate_right(root->rch);
        return rotate_left(root);
    }
    return root;
}

AVL* avl_find(AVL* root, int id, long long* cmp) {
    if (!root) return NULL;
    (*cmp)++;
    if (id == root->val.id) return root;
    if (id < root->val.id) return avl_find(root->lch, id, cmp);
    return avl_find(root->rch, id, cmp);
}

AVL* min_node(AVL* n) {
    while (n->lch) n = n->lch;
    return n;
}

AVL* avl_delete(AVL* root, int id, long long* cmp) {
    if (!root) return NULL;

    (*cmp)++;
    if (id < root->val.id)
        root->lch = avl_delete(root->lch, id, cmp);
    else if (id > root->val.id)
        root->rch = avl_delete(root->rch, id, cmp);
    else {
        if (!root->lch || !root->rch) {
            AVL* tmp = root->lch ? root->lch : root->rch;
            free(root);
            return tmp;
        } else {
            AVL* tmp = min_node(root->rch);
            root->val = tmp->val;
            root->rch = avl_delete(root->rch, tmp->val.id, cmp);
        }
    }

    root->height = 1 + max_int(get_height(root->lch), get_height(root->rch));
    int bal = avl_balance(root);

    if (bal > 1 && avl_balance(root->lch) >= 0) return rotate_right(root);
    if (bal > 1 && avl_balance(root->lch) < 0) {
        root->lch = rotate_left(root->lch);
        return rotate_right(root);
    }
    if (bal < -1 && avl_balance(root->rch) <= 0) return rotate_left(root);
    if (bal < -1 && avl_balance(root->rch) > 0) {
        root->rch = rotate_right(root->rch);
        return rotate_left(root);
    }
    return root;
}

void avl_free(AVL* r) {
    if (!r) return;
    avl_free(r->lch);
    avl_free(r->rch);
    free(r);
}

// ================== Main ==================
int main() {
    int n;
    Student* src = read_csv("dataset_id_ascending.csv", &n);
    if (!src) return 1;

    printf("총 학생 수: %d명\n\n", n);

    long long cmp1 = 0, cmp2 = 0, cmp3 = 0;
    int cap1 = 16, cnt1 = 0;
    int cap2 = 16, cnt2 = 0;
    Student* ua = malloc(sizeof(Student) * cap1);
    Student* sa = malloc(sizeof(Student) * cap2);
    AVL* root = NULL;

    // 삽입
    printf("[삽입 테스트]\n");
    for (int i = 0; i < n; i++) ua_add(&ua, &cnt1, &cap1, src[i]);
    for (int i = 0; i < n; i++) sa_add(&sa, &cnt2, &cap2, src[i], &cmp2);
    for (int i = 0; i < n; i++) root = avl_insert(root, src[i], &cmp3);

    printf("비교(UA): %lld\n", cmp1);
    printf("비교(SA): %lld\n", cmp2);
    printf("비교(AVL): %lld\n\n", cmp3);

    // 탐색
    int keys[] = { src[0].id, src[n/2].id, src[n-1].id, 999999 };
    int t = 4;
    cmp1 = cmp2 = cmp3 = 0;

    for (int i = 0; i < t; i++) ua_find(ua, cnt1, keys[i], &cmp1);
    for (int i = 0; i < t; i++) sa_search(sa, cnt2, keys[i], &cmp2);
    for (int i = 0; i < t; i++) avl_find(root, keys[i], &cmp3);

    printf("[탐색 비교]\nUA: %lld, SA: %lld, AVL: %lld\n\n", cmp1, cmp2, cmp3);

    // 삭제
    cmp1 = cmp2 = cmp3 = 0;
    for (int i = 0; i < t; i++) ua_remove(ua, &cnt1, keys[i], &cmp1);
    for (int i = 0; i < t; i++) sa_remove(sa, &cnt2, keys[i], &cmp2);
    for (int i = 0; i < t; i++) root = avl_delete(root, keys[i], &cmp3);

    printf("[삭제 비교]\nUA: %lld, SA: %lld, AVL: %lld\n", cmp1, cmp2, cmp3);

    free(src);
    free(ua);
    free(sa);
    avl_free(root);
    return 0;
}
