#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

#define TREE_SIZE 1024

typedef struct {
    int items[TREE_SIZE];
    int top;
} Stack;
void initStack(Stack* s) {
    s->top = -1;
}
bool isStackEmpty(Stack* s) {
    return s->top == -1;
}
void pushToStack(Stack* s, int item) {
    if (s->top >= TREE_SIZE - 1) {
        printf("Stack Overflow\n");
        return;
    }
    s->items[++s->top] = item;
}

// 스택에서 데이터를 꺼내는 함수
int popFromStack(Stack* s) {
    if (isStackEmpty(s)) {
        return -1;
    }
    return s->items[s->top--];
}

void iterativePreOrder(char treeArray[], int maxIndex) {
    if (maxIndex < 1) return;

    Stack stack;
    initStack(&stack);

    pushToStack(&stack, 1);
    while (!isStackEmpty(&stack)) {
        int currentIndex = popFromStack(&stack);
        int rightChild = currentIndex * 2 + 1;
        int leftChild = currentIndex * 2;

        if (currentIndex > maxIndex || !isalpha(treeArray[currentIndex])) continue;
        printf("%c ", treeArray[currentIndex]);
        if (rightChild <= maxIndex && isalpha(treeArray[rightChild])) pushToStack(&stack, rightChild);
        if (leftChild <= maxIndex && isalpha(treeArray[leftChild])) pushToStack(&stack, leftChild);
    }
}

void iterativeInOrder(char treeArray[], int maxIndex) {
    Stack stack;
    initStack(&stack);

    int currentIndex = 1;
    while ((currentIndex > 0 && currentIndex <= maxIndex && isalpha(treeArray[currentIndex])) || !isStackEmpty(&stack)) {
        while (currentIndex > 0 && currentIndex <= maxIndex && isalpha(treeArray[currentIndex])) {
            pushToStack(&stack, currentIndex);
            currentIndex = currentIndex * 2;
        }
        currentIndex = popFromStack(&stack);
        if (currentIndex != -1) {
            printf("%c ", treeArray[currentIndex]);
            currentIndex = currentIndex * 2 + 1;
        }
    }
}

void iterativePostOrder(char treeArray[], int maxIndex) {
    if (maxIndex < 1) return;

    Stack s1, s2;
    initStack(&s1);
    initStack(&s2);

    pushToStack(&s1, 1);

    while (!isStackEmpty(&s1)) {
        int nodeIndex = popFromStack(&s1);
        pushToStack(&s2, nodeIndex);
        int leftChild = nodeIndex * 2;
        int rightChild = nodeIndex * 2 + 1;
        if (leftChild <= maxIndex && isalpha(treeArray[leftChild])) pushToStack(&s1, leftChild);
        if (rightChild <= maxIndex && isalpha(treeArray[rightChild])) pushToStack(&s1, rightChild);
    }

    while (!isStackEmpty(&s2)) {
        int nodeIndex = popFromStack(&s2);
        printf("%c ", treeArray[nodeIndex]);
    }
}


void parseAndBuildTree(const char* inputString, int* stringIndex, char treeArray[], int treeIndex, int* maxTreeIndex) {
    if (treeIndex >= TREE_SIZE) return;

    while (inputString[*stringIndex] && !isalpha(inputString[*stringIndex])) {
        (*stringIndex)++;
    }
    if (inputString[*stringIndex] && isalpha(inputString[*stringIndex])) {
        treeArray[treeIndex] = inputString[*stringIndex];
        if (treeIndex > *maxTreeIndex) {
            *maxTreeIndex = treeIndex;
        }
        (*stringIndex)++;
    }

    while (inputString[*stringIndex] && isspace(inputString[*stringIndex])) {
        (*stringIndex)++;
    }
    if (inputString[*stringIndex] == '(') {
        (*stringIndex)++;
        parseAndBuildTree(inputString, stringIndex, treeArray, treeIndex * 2, maxTreeIndex);
        while (inputString[*stringIndex] && isspace(inputString[*stringIndex])) {
            (*stringIndex)++;
        }
        if (inputString[*stringIndex] != ')') {
            parseAndBuildTree(inputString, stringIndex, treeArray, treeIndex * 2 + 1, maxTreeIndex);
        }
        while (inputString[*stringIndex] && inputString[*stringIndex] != ')') {
            (*stringIndex)++;
        }
        if (inputString[*stringIndex] == ')') {
             (*stringIndex)++;
        }
    }
}


int main(void) {
    char inputString[TREE_SIZE];
    char treeArray[TREE_SIZE];
    int stringPosition = 0;
    int maxUsedIndex = 0;

    if (scanf("%[^\n]", inputString) != 1) return 1;
    for (int i = 0; i < TREE_SIZE; ++i) treeArray[i] = '\0';
    parseAndBuildTree(inputString, &stringPosition, treeArray, 1, &maxUsedIndex);

    printf("pre-order: ");
    iterativePreOrder(treeArray, maxUsedIndex);
    printf("\nin-order: ");
    iterativeInOrder(treeArray, maxUsedIndex);
    printf("\npost-order: ");
    iterativePostOrder(treeArray, maxUsedIndex);

    return 0;
}

/*
pre-order: A B C D E G H I J K L M N
in-order: C B D A G E I H L K J N M
post-order: C D B G I L K N M J H E A
*/