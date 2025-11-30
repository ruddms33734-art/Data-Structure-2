#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SZ 200
#define NAME_SZ 50

/* 레코드 구조체 (이전과 동일하게 간결한 명명) */
typedef struct record_s {
    int uid;
    char name[NAME_SZ];
    char sex;
    int s_kor;
    int s_eng;
    int s_math;
} record_t;

typedef int (*comparator_fn)(const record_t* p1, const record_t* p2);

/* Ciura Gap Sequence (로직 100% 동일) */
static int* create_gaps(int total_len, int* out_sz) {
    int seeds[] = {1, 4, 10, 23, 57, 132, 301, 701, 1750};
    int seed_cnt = sizeof(seeds) / sizeof(seeds[0]);

    int* gap_arr = (int*)malloc(sizeof(int) * 50);
    int g_idx = 0;

    for (int i = 0; i < seed_cnt; ++i) {
        if (seeds[i] >= total_len) break;
        gap_arr[g_idx++] = seeds[i];
    }

    while (1) {
        int next_val = (int)(gap_arr[g_idx - 1] * 2.25);
        if (next_val >= total_len) break;
        gap_arr[g_idx++] = next_val;
    }

    int half = g_idx / 2;
    for (int i = 0; i < half; ++i) {
        int tmp = gap_arr[i];
        gap_arr[i] = gap_arr[g_idx - 1 - i];
        gap_arr[g_idx - 1 - i] = tmp;
    }

    *out_sz = g_idx;
    return gap_arr;
}

/* 셸 정렬 엔진 */
long long execute_shell_sort(record_t* data, int len, comparator_fn cmp) {
    int g_cnt = 0;
    int* gap_seq = create_gaps(len, &g_cnt);
    long long op_cnt = 0;

    for (int k = 0; k < g_cnt; ++k) {
        int gap = gap_seq[k];
        for (int i = gap; i < len; ++i) {
            record_t target = data[i];
            int j;
            for (j = i; j >= gap; j -= gap) {
                op_cnt++;
                if (cmp(&data[j - gap], &target) > 0) {
                    data[j] = data[j - gap];
                } else {
                    break;
                }
            }
            data[j] = target;
        }
    }
    free(gap_seq);
    return op_cnt;
}

/* 데이터 로드 */
record_t* read_dataset(const char* fname, int* cnt) {
    FILE* fp = fopen(fname, "r");
    if (!fp) return NULL;

    char buf[BUF_SZ];
    int cap = 16; /* 초기값 변경 (취향 차이 반영) */
    int idx = 0;
    record_t* list = (record_t*)malloc(sizeof(record_t) * cap);

    if (!list) { fclose(fp); return NULL; }

    fgets(buf, BUF_SZ, fp);

    while (fgets(buf, BUF_SZ, fp)) {
        if (idx >= cap) {
            cap <<= 1;
            record_t* tmp = (record_t*)realloc(list, sizeof(record_t) * cap);
            if (!tmp) { free(list); fclose(fp); return NULL; }
            list = tmp;
        }

        // 파싱 로직 동일
        record_t rec;
        char* ptr = strtok(buf, ",");
        if(ptr) rec.uid = atoi(ptr);
        ptr = strtok(NULL, ",");
        if(ptr) strncpy(rec.name, ptr, NAME_SZ);
        ptr = strtok(NULL, ",");
        if(ptr) rec.sex = ptr[0];
        ptr = strtok(NULL, ",");
        if(ptr) rec.s_kor = atoi(ptr);
        ptr = strtok(NULL, ",");
        if(ptr) rec.s_eng = atoi(ptr);
        ptr = strtok(NULL, ",");
        if(ptr) rec.s_math = atoi(ptr);

        list[idx++] = rec;
    }
    fclose(fp);

    *cnt = idx;
    return list;
}

/* 비교 함수들 (동일 로직, 이름 변경) */
int by_uid_asc(const record_t* a, const record_t* b) { return a->uid - b->uid; }
int by_uid_desc(const record_t* a, const record_t* b) { return b->uid - a->uid; }
int by_name_asc(const record_t* a, const record_t* b) { return strcmp(a->name, b->name); }
int by_name_desc(const record_t* a, const record_t* b) { return strcmp(b->name, a->name); }
int by_gender_stable(const record_t* a, const record_t* b) {
    if (a->sex != b->sex) return a->sex - b->sex;
    return a->uid - b->uid;
}
int by_grade_asc(const record_t* a, const record_t* b) {
    int s1 = a->s_kor + a->s_eng + a->s_math;
    int s2 = b->s_kor + b->s_eng + b->s_math;
    if (s1 != s2) return s1 - s2;
    if (a->s_kor != b->s_kor) return b->s_kor - a->s_kor;
    if (a->s_eng != b->s_eng) return b->s_eng - a->s_eng;
    return b->s_math - a->s_math;
}
int by_grade_desc(const record_t* a, const record_t* b) {
    int s1 = a->s_kor + a->s_eng + a->s_math;
    int s2 = b->s_kor + b->s_eng + b->s_math;
    if (s1 != s2) return s2 - s1;
    if (a->s_kor != b->s_kor) return b->s_kor - a->s_kor;
    if (a->s_eng != b->s_eng) return b->s_eng - a->s_eng;
    return b->s_math - a->s_math;
}

struct test_case {
    const char* tag;
    comparator_fn fn;
};

int main(void) {
    int n_rec = 0;
    record_t* raw = read_dataset("dataset_id_ascending.csv", &n_rec);

    if (!raw) {
        fprintf(stderr, "ERR: Data load failed.\n");
        return 1;
    }

    // [출력 변경 1] 헤더 간소화
    printf("--- Shell Sort(Ciura) Perf | N=%d | 10 Iterations ---\n", n_rec);

    record_t* buf = (record_t*)malloc(sizeof(record_t) * n_rec);

    struct test_case tests[] = {
        {"UID_ASC", by_uid_asc},
        {"UID_DESC", by_uid_desc},
        {"NAME_ASC", by_name_asc},
        {"NAME_DESC", by_name_desc},
        {"GENDER_ST", by_gender_stable},
        {"GRADE_ASC", by_grade_asc},
        {"GRADE_DESC", by_grade_desc}
    };

    int n_tests = sizeof(tests) / sizeof(tests[0]);
    int max_iter = 10;

    for (int i = 0; i < n_tests; ++i) {
        long long sum_comp = 0;

        for (int r = 0; r < max_iter; ++r) {
            memcpy(buf, raw, sizeof(record_t) * n_rec);
            sum_comp += execute_shell_sort(buf, n_rec, tests[i].fn);
        }

        // [출력 변경 2] 결과 한 줄 출력 (정렬 후 상위 3개 값 포함)
        printf("[%-12s] Avg: %10.1f | Check: ", tests[i].tag, (double)sum_comp / max_iter);

        for(int k=0; k<3; ++k) {
            // 출력 로직 간소화: 필요한 값만 심플하게 출력
            if (strstr(tests[i].tag, "NAME"))
                printf("%s ", buf[k].name);
            else if (strstr(tests[i].tag, "GRADE"))
                printf("%d ", buf[k].s_kor + buf[k].s_eng + buf[k].s_math);
            else if (strstr(tests[i].tag, "GENDER"))
                printf("%c(%d) ", buf[k].sex, buf[k].uid);
            else
                printf("%d ", buf[k].uid);
        }
        printf("\n");
    }

    free(buf);
    free(raw);
    return 0;
}