#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

// 파싱 결과를 담을 구조체
typedef struct {
    // 성공 시 NULL, 에러 발생 시 해당 에러 메시지를 가리킴
    const char* error_message;
} ParseResult;

// 에러 메시지를 상수로 정의
const char* const ERROR_CHILD_LIMIT = "Child Limit Error: A node has more than two atom children.";


ParseResult parse_list(const char input[], int *cursor) {
    int atom_child_count = 0;

    while (input[*cursor] != ')' && input[*cursor] != '\0') {
        while (isspace(input[*cursor])) {
            (*cursor)++;
        }

        if (input[*cursor] == ')' || input[*cursor] == '\0') {
            break;
        }

        if (input[*cursor] == '(') {
            (*cursor)++;
            ParseResult sub_result = parse_list(input, cursor);
            if (sub_result.error_message != NULL) {
                return sub_result;
            }
        } else {
            atom_child_count++;
            (*cursor)++;
        }
    }

    if (input[*cursor] == ')') {
        (*cursor)++;
        if (atom_child_count > 2) {
            return (ParseResult){.error_message = ERROR_CHILD_LIMIT};
        } else {
            return (ParseResult){.error_message = NULL};
        }
    }

    return (ParseResult){.error_message = NULL};
}


bool is_valid_binary_tree_format(const char input[]) {
    int cursor = 0;

    while (isspace(input[cursor])) {
        cursor++;
    }

    cursor++; // '('를 소모합니다.

    ParseResult result = parse_list(input, &cursor);

    if (result.error_message != NULL) {
        return false;
    }

    return true;
}

int main(void) {
    char buffer[256];

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        buffer[0] = '\0';
    }

    char *newline = strchr(buffer, '\n');
    if (newline != NULL) {
        *newline = '\0';
    }

    if (is_valid_binary_tree_format(buffer)) {
        printf("TRUE");
    } else {
        printf("FALSE");
    }

    return 0;
}