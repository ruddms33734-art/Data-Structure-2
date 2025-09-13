#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NODES 200
#define MAX_LEN   1000

typedef struct Node {
    char value[10];
    struct Node **children;
    int childCount;
} Node;

char *tree_array[MAX_NODES];

char *tokens[MAX_LEN];
int tokenCount = 0;
int pos = 0;

int tree_height(Node *node) {
    if (!node) return -1;
    if (node->childCount == 0) return 0;

    int maxChildHeight = 0;
    for (int i = 0; i < node->childCount; i++) {
        int h = tree_height(node->children[i]);
        if (h > maxChildHeight) maxChildHeight = h;
    }
    return maxChildHeight + 1;
}

int total_nodes(Node *node) {
    if (!node) return 0;
    int count = 1;
    for (int i = 0; i < node->childCount; i++)
        count += total_nodes(node->children[i]);
    return count;
}

int leaf_nodes(Node *node) {
    if (!node) return 0;
    if (node->childCount == 0) return 1;

    int count = 0;
    for (int i = 0; i < node->childCount; i++)
        count += leaf_nodes(node->children[i]);
    return count;
}

void scanExpression(const char *expr) {
    tokenCount = 0;
    const char *p = expr;
    while (*p) {
        if (*p == '(' || *p == ')') {
            char *tok = (char*)malloc(2);
            tok[0] = *p; tok[1] = '\0';
            tokens[tokenCount++] = tok;
            p++;
        } else if (isspace(*p)) {
            p++;
        } else if (isalpha(*p)) {
            char *tok = (char*)malloc(2);
            tok[0] = *p; tok[1] = '\0';
            tokens[tokenCount++] = tok;
            p++;
        } else {
            p++;
        }
    }
}

void free_tokens() {
    for (int i = 0; i < tokenCount; i++) {
        free(tokens[i]);
        tokens[i] = NULL;
    }
    tokenCount = 0;
}

Node* parse() {
    if (pos >= tokenCount) return NULL;

    char *tok = tokens[pos++];

    if (strcmp(tok, "(") == 0) {
        Node *root = parse();
        return root;
    } else if (strcmp(tok, ")") == 0) {
        return NULL;
    } else {
        Node *node = (Node*)malloc(sizeof(Node));
        strcpy(node->value, tok);
        node->children = (Node**)malloc(sizeof(Node*) * MAX_NODES);
        node->childCount = 0;

        while (pos < tokenCount && strcmp(tokens[pos], "(") == 0) {
            pos++; // "(" consume
            Node *child;
            while ((child = parse()) != NULL) {
                node->children[node->childCount++] = child;
            }
        }
        return node;
    }
}

void dfs(Node *node, int idx) {
    if (!node) return;
    tree_array[idx] = strdup(node->value);
    for (int j = 0; j < node->childCount; j++) {
        dfs(node->children[j], idx * 2 + j);
    }
}

void free_tree(Node *node) {
    if (!node) return;
    for (int i = 0; i < node->childCount; i++)
        free_tree(node->children[i]);
    free(node->children);
    free(node);
}

void made_tree(const char *expr) {
    scanExpression(expr);
    pos = 0;
    Node *root = parse();

    for (int i = 0; i < MAX_NODES; i++)
        tree_array[i] = NULL;

    dfs(root, 1);

    printf("%d, %d, %d\n", tree_height(root), total_nodes(root), leaf_nodes(root));

    free_tree(root);
    free_tokens();
}

int main() {
    char expr[MAX_LEN];

    if (scanf(" %[^\n]", expr) == 1) {
        made_tree(expr);
    } else {
        printf("ERROR\n");
    }

    for (int i = 0; i < MAX_NODES; i++) {
        if (tree_array[i]) free(tree_array[i]);
    }

    return 0;
}
