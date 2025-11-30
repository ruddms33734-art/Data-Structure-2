#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constants */
#define BUFFER_SIZE 1024
#define NAME_LIMIT 64
#define CUTOFF_THRESHOLD 12

/* Data Structure - Gender Removed */
typedef struct {
    int id;
    char name[NAME_LIMIT];
    // char sex; removed
    int score_kor;
    int score_eng;
    int score_math;
} Record;

/* Function Pointer Type for Comparison (Generic Style) */
typedef int (*Comparator)(const void*, const void*);

/* Utility: Inline Swap */
static inline void swap(Record* a, Record* b) {
    Record temp = *a;
    *a = *b;
    *b = temp;
}

/* Helper: Insertion Sort for small arrays */
void run_insertion_sort(Record* base, int left, int right, Comparator cmp, long long* metric) {
    for (int i = left + 1; i <= right; ++i) {
        Record target = base[i];
        int j = i;
        while (j > left) {
            (*metric)++;
            if (cmp(&base[j - 1], &target) > 0) {
                base[j] = base[j - 1];
                j--;
            } else {
                break;
            }
        }
        base[j] = target;
    }
}

/* Core Logic: Dual-Pivot Partitioning */
void execute_sort(Record* list, int left, int right, Comparator cmp, long long* cnt) {
    int len = right - left + 1;

    if (len < CUTOFF_THRESHOLD) {
        run_insertion_sort(list, left, right, cmp, cnt);
        return;
    }

    // Median-of-5 pivot candidates
    int div = len / 6;
    int idx[5] = { left + div, left + 2 * div, left + len / 2, right - 2 * div, right - div };

    // Sorting network
    if (cmp(&list[idx[0]], &list[idx[1]]) > 0) { (*cnt)++; swap(&list[idx[0]], &list[idx[1]]); } else (*cnt)++;
    if (cmp(&list[idx[3]], &list[idx[4]]) > 0) { (*cnt)++; swap(&list[idx[3]], &list[idx[4]]); } else (*cnt)++;
    if (cmp(&list[idx[0]], &list[idx[2]]) > 0) { (*cnt)++; swap(&list[idx[0]], &list[idx[2]]); } else (*cnt)++;
    if (cmp(&list[idx[1]], &list[idx[2]]) > 0) { (*cnt)++; swap(&list[idx[1]], &list[idx[2]]); } else (*cnt)++;
    if (cmp(&list[idx[0]], &list[idx[3]]) > 0) { (*cnt)++; swap(&list[idx[0]], &list[idx[3]]); } else (*cnt)++;
    if (cmp(&list[idx[2]], &list[idx[3]]) > 0) { (*cnt)++; swap(&list[idx[2]], &list[idx[3]]); } else (*cnt)++;
    if (cmp(&list[idx[1]], &list[idx[4]]) > 0) { (*cnt)++; swap(&list[idx[1]], &list[idx[4]]); } else (*cnt)++;
    if (cmp(&list[idx[1]], &list[idx[2]]) > 0) { (*cnt)++; swap(&list[idx[1]], &list[idx[2]]); } else (*cnt)++;
    if (cmp(&list[idx[3]], &list[idx[4]]) > 0) { (*cnt)++; swap(&list[idx[3]], &list[idx[4]]); } else (*cnt)++;

    swap(&list[left], &list[idx[1]]);
    swap(&list[right], &list[idx[3]]);

    Record pivot1 = list[left];
    Record pivot2 = list[right];

    int lt = left + 1;
    int gt = right - 1;
    int iter = lt;

    while (iter <= gt) {
        (*cnt)++;
        if (cmp(&list[iter], &pivot1) < 0) {
            swap(&list[iter], &list[lt]);
            lt++;
        }
        else {
            (*cnt)++;
            if (cmp(&list[iter], &pivot2) > 0) {
                while (iter < gt) {
                    (*cnt)++;
                    if (cmp(&list[gt], &pivot2) > 0) {
                        gt--;
                    } else {
                        break;
                    }
                }
                swap(&list[iter], &list[gt]);
                gt--;

                (*cnt)++;
                if (cmp(&list[iter], &pivot1) < 0) {
                    swap(&list[iter], &list[lt]);
                    lt++;
                }
            }
        }
        iter++;
    }

    lt--;
    gt++;
    swap(&list[left], &list[lt]);
    swap(&list[right], &list[gt]);

    execute_sort(list, left, lt - 1, cmp, cnt);

    (*cnt)++;
    if (cmp(&pivot1, &pivot2) < 0) {
        execute_sort(list, lt + 1, gt - 1, cmp, cnt);
    }

    execute_sort(list, gt + 1, right, cmp, cnt);
}

/* File IO: CSV Parser */
Record* parse_csv_file(const char* path, int* record_count) {
    FILE* stream = fopen(path, "r");
    if (!stream) {
        fprintf(stderr, "[Error] Cannot open file: %s\n", path);
        return NULL;
    }

    int size = 32;
    int n = 0;
    Record* db = (Record*)malloc(sizeof(Record) * size);
    char buffer[BUFFER_SIZE];

    if (!fgets(buffer, BUFFER_SIZE, stream)) {
        fclose(stream);
        free(db);
        return NULL;
    }

    while (fgets(buffer, BUFFER_SIZE, stream)) {
        if (n == size) {
            size <<= 1;
            Record* temp = realloc(db, sizeof(Record) * size);
            if (!temp) {
                free(db);
                fclose(stream);
                return NULL;
            }
            db = temp;
        }

        Record* r = &db[n];

        char* ptr = strtok(buffer, ",");
        if(ptr) r->id = atoi(ptr);

        ptr = strtok(NULL, ",");
        if(ptr) strncpy(r->name, ptr, NAME_LIMIT - 1);
        r->name[NAME_LIMIT - 1] = '\0';

        // Important: Skip the Gender column in CSV
        ptr = strtok(NULL, ",");
        // Do not assign to struct

        ptr = strtok(NULL, ",");
        if(ptr) r->score_kor = atoi(ptr);

        ptr = strtok(NULL, ",");
        if(ptr) r->score_eng = atoi(ptr);

        ptr = strtok(NULL, ",");
        if(ptr) r->score_math = atoi(ptr);

        n++;
    }

    fclose(stream);
    *record_count = n;
    return db;
}

/* Comparison Functions */
int compare_id_asc(const void* a, const void* b) {
    return ((Record*)a)->id - ((Record*)b)->id;
}

int compare_id_desc(const void* a, const void* b) {
    return ((Record*)b)->id - ((Record*)a)->id;
}

int compare_name_asc(const void* a, const void* b) {
    return strcmp(((Record*)a)->name, ((Record*)b)->name);
}

int compare_name_desc(const void* a, const void* b) {
    return strcmp(((Record*)b)->name, ((Record*)a)->name);
}

// compare_gender_stable removed

int compare_total_grade_asc(const void* a, const void* b) {
    const Record* ra = (const Record*)a;
    const Record* rb = (const Record*)b;
    int sumA = ra->score_kor + ra->score_eng + ra->score_math;
    int sumB = rb->score_kor + rb->score_eng + rb->score_math;

    if (sumA != sumB) return sumA - sumB;
    if (ra->score_kor != rb->score_kor) return rb->score_kor - ra->score_kor;
    if (ra->score_eng != rb->score_eng) return rb->score_eng - ra->score_eng;
    return rb->score_math - ra->score_math;
}

int compare_total_grade_desc(const void* a, const void* b) {
    const Record* ra = (const Record*)a;
    const Record* rb = (const Record*)b;
    int sumA = ra->score_kor + ra->score_eng + ra->score_math;
    int sumB = rb->score_kor + rb->score_eng + rb->score_math;

    if (sumA != sumB) return sumB - sumA;
    if (ra->score_kor != rb->score_kor) return rb->score_kor - ra->score_kor;
    if (ra->score_eng != rb->score_eng) return rb->score_eng - ra->score_eng;
    return rb->score_math - ra->score_math;
}

/* Main Execution */
int main() {
    const char* filename = "dataset_id_ascending.csv";
    int num_records = 0;

    Record* source_data = parse_csv_file(filename, &num_records);

    if (!source_data) {
        return EXIT_FAILURE;
    }

    printf("=== Performance Analysis: Dual-Pivot Quick Sort ===\n");
    printf("Dataset Size: %d records\n", num_records);
    printf("Iterations per case: 10\n\n");

    Record* buffer = (Record*)malloc(sizeof(Record) * num_records);

    typedef struct {
        const char* desc;
        Comparator cmp_fn;
    } TestCase;

    // Gender test case removed
    TestCase cases[] = {
        {"Sort by ID (Asc)", compare_id_asc},
        {"Sort by ID (Desc)", compare_id_desc},
        {"Sort by Name (Asc)", compare_name_asc},
        {"Sort by Name (Desc)", compare_name_desc},
        {"Sort by Grades (Asc)", compare_total_grade_asc},
        {"Sort by Grades (Desc)", compare_total_grade_desc}
    };

    int total_cases = sizeof(cases) / sizeof(TestCase);
    const int LOOPS = 10;

    for (int i = 0; i < total_cases; ++i) {
        long long ops_count = 0;

        for (int j = 0; j < LOOPS; ++j) {
            memcpy(buffer, source_data, sizeof(Record) * num_records);
            execute_sort(buffer, 0, num_records - 1, cases[i].cmp_fn, &ops_count);
        }

        printf("[Case %d] %s\n", i + 1, cases[i].desc);
        printf(" -> Avg Comparisons: %.1f\n\n", (double)ops_count / LOOPS);
    }

    free(buffer);
    free(source_data);

    return EXIT_SUCCESS;
}
