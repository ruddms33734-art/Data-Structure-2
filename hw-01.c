#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    STATUS_OPEN,
    STATUS_CLOSE,
    STATUS_ROOT
} ParseStatus;

int validate_root_label(const char input[], int *cursor) {
    int index = 0;
    int depthLevel = 0;
    int rootLabelCount = 0;
    int rootLabelActive = 0;

    while (input[index] != '\0') {
        unsigned char character = (unsigned char) input[index];

        if (character == '(') {
            depthLevel++;
            rootLabelActive = 0;
            index++;
            continue;
        }

        if (character == ')') {
            if (depthLevel > 0) depthLevel--;
            rootLabelActive = 0;
            index++;
            continue;
        }

        if (isspace(character)) {
            index++;
            continue;
        }

        if (isalpha(character) && depthLevel == 1) {
            if (rootLabelActive) {
                return 0; // More than one root label detected
            }
            rootLabelCount++;
            rootLabelActive = 1;

            while (input[index] != '\0' && isalpha((unsigned char) input[index])) {
                index++;
                if (isalpha((unsigned char) input[index])) {
                    return 0; // Multiple root labels detected
                }
            }
            continue;
        }
        index++;
    }
    *cursor = index;
    return (rootLabelCount == 1) ? 1 : 0;
}

int validate_parentheses(const char input[], int *cursor) {
    int openCount = 0;
    int closeCount = 0;

    while (input[*cursor] != '\0') {
        if (input[*cursor] == '(') {
            openCount++;
            (*cursor)++;
        } else if (input[*cursor] == ')') {
            if (openCount < closeCount) return 0;
            closeCount++;
            (*cursor)++;
        } else {
            (*cursor)++;
        }
    }
    return (openCount == closeCount);
}

int validate_tree(const char input[], int *cursor, ParseStatus status) {
    int childCount = 0;

    while (input[*cursor] != '\0') {
        while (isspace(input[*cursor])) (*cursor)++;

        if (input[*cursor] == ')') {
            if (status != STATUS_OPEN) return -1;
            status = STATUS_CLOSE;
            (*cursor)++;
            break;
        }

        if (input[*cursor] == '(') {
            (*cursor)++;
            int subtreeCount = validate_tree(input, cursor, STATUS_OPEN);
            if (subtreeCount > 2 || subtreeCount == -1) return subtreeCount;
            continue;
        }

        childCount++;
        (*cursor)++;
    }

    if (status == STATUS_OPEN) return -1;
    return childCount;
}

int main(void) {
    char buffer[256];
    printf("Input:");
    scanf("%[^\n]", buffer);

    int cursor = 0;
    int validationResult = -1;

    while (buffer[cursor] != '\0') {
        if (buffer[cursor] == '\n') {
            printf("\nOutput: ERROR\n");
            return 0;
        }
        cursor++;
    }

    cursor = 0;
    if (validate_parentheses(buffer, &cursor) && validate_root_label(buffer, &cursor)) {
        cursor = 0;
        validationResult = validate_tree(buffer, &cursor, STATUS_ROOT);
    }

    if (validationResult == -1) {
        printf("\nOutput: ERROR\n");
    } else if (validationResult < 2) {
        printf("\nOutput: TRUE\n");
    } else if (validationResult > 2) {
        printf("\nOutput: FALSE\n");
    }
    return 0;
}
