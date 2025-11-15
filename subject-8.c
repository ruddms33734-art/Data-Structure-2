#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define DATA_SIZE 10000
#define MAX_VAL 1000000
#define MAX_VAL_MODULO (MAX_VAL + 1)
#define NUM_TRIALS 100

void generate_data(int *arr) {
    for (int i = 0; i < DATA_SIZE; i++) {
        arr[i] = rand() % MAX_VAL_MODULO;
    }
}

long long insertion_sort(int *arr, int n) {
    long long comparisons = 0;

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0) {
            comparisons++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            } else {
                break;
            }
        }
        arr[j + 1] = key;
    }
    return comparisons;
}

long long shell_sort_basic(int *arr, int n) {
    long long comparisons = 0;

    for (int gap = n / 2; gap > 0; gap /= 2) {

        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;

            for (j = i; j >= gap; j -= gap) {
                comparisons++;
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                } else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }
    return comparisons;
}

long long shell_sort_ciura(int *arr, int n) {
    long long comparisons = 0;

    int gaps[] = {8859, 3937, 1750, 701, 301, 132, 57, 23, 10, 4, 1};
    int num_gaps = sizeof(gaps) / sizeof(gaps[0]);

    for (int k = 0; k < num_gaps; k++) {
        int gap = gaps[k];
        if (gap >= n) continue;

        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;

            for (j = i; j >= gap; j -= gap) {
                comparisons++;
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                } else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }
    return comparisons;
}

int main() {
    srand((unsigned int)time(NULL));

    setlocale(LC_NUMERIC, "");

    long long total_insertion_comps = 0;
    long long total_shell_basic_comps = 0;
    long long total_shell_ciura_comps = 0;

    int original_data[DATA_SIZE];
    int data_insertion[DATA_SIZE];
    int data_shell_basic[DATA_SIZE];
    int data_shell_ciura[DATA_SIZE];

    printf("데이터 크기: %d, 실행 횟수: %d\n", DATA_SIZE, NUM_TRIALS);
    printf("----------------------------------------\n");
    fflush(stdout);

    for (int i = 0; i < NUM_TRIALS; i++) {
        generate_data(original_data);

        memcpy(data_insertion, original_data, sizeof(original_data));
        memcpy(data_shell_basic, original_data, sizeof(original_data));
        memcpy(data_shell_ciura, original_data, sizeof(original_data));

        total_insertion_comps += insertion_sort(data_insertion, DATA_SIZE);
        total_shell_basic_comps += shell_sort_basic(data_shell_basic, DATA_SIZE);
        total_shell_ciura_comps += shell_sort_ciura(data_shell_ciura, DATA_SIZE);

        if ((i + 1) % 10 == 0) {
            printf("진행 중... (%d/%d)\n", i + 1, NUM_TRIALS);
            fflush(stdout);
        }
    }

    double avg_insertion = (double)total_insertion_comps / NUM_TRIALS;
    double avg_shell_basic = (double)total_shell_basic_comps / NUM_TRIALS;
    double avg_shell_ciura = (double)total_shell_ciura_comps / NUM_TRIALS;

    printf("\n--- 최종 결과 (100회 평균 비교 횟수) ---\n");
    printf("%-22s: %'15.0f 회\n", "1. 단순 삽입 정렬    ", avg_insertion);
    printf("%-22s: %'15.0f 회\n", "2. 기본 쉘 정렬 (N/2)", avg_shell_basic);
    printf("%-22s: %'15.0f 회\n", "3. Ciura 간격 쉘 정렬", avg_shell_ciura);

    return 0;
}