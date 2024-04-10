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
    ND_RETURN,
    ND_IFSTMT, // ND_IFSTMT(lhs: ND_IFBRANCH(lhs:if-cond, rhs: if-stmt), rhs: else-stmt)
    ND_IFBRANCH, // ND_IFBRANCH(lhs:if-cond, rhs: if-stmt)
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

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};


void parse_program(Token *tok);
Node *parse(Token *tok);

extern Node *code[100];