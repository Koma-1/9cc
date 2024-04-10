#pragma once

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NEQ,
    ND_LT,
    ND_LEQ,
    ND_GT,
    ND_GEQ,
    ND_ASSIGN,
    ND_LVAR,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

void parse_program(Token *tok);
Node *parse(Token *tok);

extern Node *code[100];