#include <stdio.h>
#include <ctype.h>
#include <stdbool.h> // bool 타입을 사용하기 위해 포함

// 파싱 결과를 나타내기 위한 열거형(enum)
// 성공, 구조적 오류(자식 노드가 2개 초과), 문법 오류를 명확히 구분
typedef enum {
    VALID,
    INVALID_STRUCTURE, // 자식 노드가 2개를 초과한 경우
    SYNTAX_ERROR       // 괄호 불일치 등 형식에 맞지 않는 경우
} ParseResult;

// 전방 선언 (Recursive call을 위해 필요)
ParseResult parse_node(const char **cursor);

// 공백 문자를 건너뛰는 헬퍼 함수
void skip_whitespace(const char **cursor) {
    while (isspace(**cursor)) {
        (*cursor)++;
    }
}

// 리스트(괄호로 묶인 부분)를 파싱하는 함수
// 예: (A (B C))
ParseResult parse_list(const char **cursor) {
    // '('로 시작하므로 다음 문자로 이동
    (*cursor)++;
    
    int child_count = 0;
    while (true) {
        skip_whitespace(cursor);

        // ')'를 만나면 리스트의 끝
        if (**cursor == ')') {
            (*cursor)++; // ')'를 소모하고 루프 종료
            break;
        }

        // 닫는 괄호가 나오기 전에 문자열이 끝나면 문법 오류
        if (**cursor == '\0') {
            return SYNTAX_ERROR;
        }

        // 자식 노드 파싱
        ParseResult result = parse_node(cursor);
        if (result != VALID) {
            return result; // 하위 노드에서 오류 발생 시 그대로 반환
        }
        child_count++;
    }

    // 각 노드는 최대 2개의 자식만 가질 수 있다는 규칙 검사
    if (child_count > 2) {
        return INVALID_STRUCTURE;
    }

    return VALID;
}

// 노드를 파싱하는 핵심 재귀 함수
// 노드는 알파벳(atom)이거나 다른 리스트(list)일 수 있음
ParseResult parse_node(const char **cursor) {
    skip_whitespace(cursor);

    if (isalpha(**cursor)) {
        // 노드가 알파벳(atom)인 경우, 해당 문자를 소모하고 성공 반환
        (*cursor)++;
        return VALID;
    } else if (**cursor == '(') {
        // 노드가 리스트인 경우, parse_list 함수로 처리
        return parse_list(cursor);
    }

    // 알파벳이나 '('로 시작하지 않으면 문법 오류
    return SYNTAX_ERROR;
}

// 전체 문자열의 유효성을 검사하는 메인 함수
ParseResult is_valid_binary_tree_format(const char *str) {
    const char *cursor = str;

    // 첫 번째 노드(루트) 파싱 시작
    ParseResult result = parse_node(&cursor);
    if (result != VALID) {
        return result;
    }

    // 파싱이 끝난 후, 문자열의 끝에 추가적인 문자가 남아있는지 확인
    skip_whitespace(&cursor);
    if (*cursor != '\0') {
        return SYNTAX_ERROR; // 예: (A B) C 와 같은 경우
    }

    return VALID;
}

int main(void) {
    char str[128];
    printf("Input: ");
    // 한 줄 전체를 입력받음
    if (scanf("%[^\n]", str) != 1) {
        // 빈 입력 처리
        printf("\nERROR\n");
        return 1;
    }

    ParseResult result = is_valid_binary_tree_format(str);

    printf("\nOutput: ");
    switch (result) {
        case VALID:
            printf("TRUE\n");
            break;
        case INVALID_STRUCTURE:
            printf("FALSE\n");
            break;
        case SYNTAX_ERROR:
            printf("ERROR\n");
            break;
    }

    return 0;
}