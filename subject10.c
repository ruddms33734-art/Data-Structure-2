#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEARNERS 35000

typedef struct {
    int id;
    char name[50];
    char gender[10];
    int koreanGrade;
    int englishGrade;
    int mathGrade;
    long long product;
} Learner;

Learner learners[MAX_LEARNERS];
int learner_count = 0;

long long sort_comparisons = 0;

FILE* openFile(const char* filePath, const char* mode) {
    FILE* fp = fopen(filePath, mode);
    if (fp == NULL) {
        printf("file open fail: %s\n", filePath);
    }
    return fp;
}

long long calculateProduct(int koreanGrade, int englishGrade, int mathGrade) {
    return (long long)koreanGrade * englishGrade * mathGrade;
}

void loadLearnerData(const char* filePath) {
    FILE* fp = openFile(filePath, "r");
    if (fp == NULL) return;

    char buffer[1024];
    fgets(buffer, sizeof(buffer), fp);

    int i = 0;
    while (fscanf(fp, "%d,%[^,],%[^,],%d,%d,%d",
                  &learners[i].id, learners[i].name, learners[i].gender,
                  &learners[i].koreanGrade, &learners[i].englishGrade, &learners[i].mathGrade) != EOF) {
        learners[i].product = calculateProduct(learners[i].koreanGrade, learners[i].englishGrade, learners[i].mathGrade);
        i++;
        if (i >= MAX_LEARNERS) break;
    }
    learner_count = i;
    fclose(fp);
}

void performLinearSearch(long long searchTarget) {
    printf("\n      [순차 탐색]       \n");
    int comparisons = 0;
    int found = 0;

    for (int i = 0; i < learner_count; i++) {
        comparisons++;
        if (learners[i].product == searchTarget) {
            found = 1;
            break;
        }
    }

    printf("비교 횟수: %d회\n", comparisons);
    printf("-----------------------------------\n");
    printf("결과: %s\n", found ? "찾음" : "못 찾음");
}

int partition(Learner arr[], int lowIndex, int highIndex) {
    long long pivot = arr[highIndex].product;
    int i = (lowIndex - 1);

    for (int j = lowIndex; j < highIndex; j++) {
        sort_comparisons++;
        if (arr[j].product < pivot) {
            i++;
            Learner temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    Learner temp = arr[i + 1];
    arr[i + 1] = arr[highIndex];
    arr[highIndex] = temp;
    return (i + 1);
}

void quickSort(Learner arr[], int lowIndex, int highIndex) {
    if (lowIndex < highIndex) {
        int pi = partition(arr, lowIndex, highIndex);
        quickSort(arr, lowIndex, pi - 1);
        quickSort(arr, pi + 1, highIndex);
    }
}

void performBinarySearch(long long searchTarget) {
    printf("\n    [정렬 후 이진 탐색]\n");

    sort_comparisons = 0;
    quickSort(learners, 0, learner_count - 1);
    printf("퀵 정렬 완료 (정렬 비교 횟수: %lld회)\n", sort_comparisons);

    int search_comparisons = 0;
    int found = 0;
    int leftIndex = 0, rightIndex = learner_count - 1;

    while (leftIndex <= rightIndex) {
        search_comparisons++;
        int midIndex = (leftIndex + rightIndex) / 2;

        if (learners[midIndex].product == searchTarget) {
            found = 1;
            break;
        }
        else if (learners[midIndex].product < searchTarget) {
            leftIndex = midIndex + 1;
        }
        else {
            rightIndex = midIndex - 1;
        }
    }

    printf("이진 탐색 완료 (탐색 비교 횟수: %d회)\n", search_comparisons);
    printf("정렬 + 탐색: %lld회\n", sort_comparisons + search_comparisons);
    printf("-----------------------------------\n");
    printf("결과: %s\n", found ? "찾음" : "못 찾음");
}

int main() {
    srand(time(NULL));

    loadLearnerData("dataset_id_ascending.csv");
    printf("데이터 %d개 로드 완료\n", learner_count);

    long long searchTarget = rand() % 1000001;
    // long long searchTarget = 4410;
    printf("목표 값(Target): %lld\n", searchTarget);

    performLinearSearch(searchTarget);
    performBinarySearch(searchTarget);

    return 0;
}
