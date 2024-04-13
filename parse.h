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
    ND_RETURN, // ND_RETURN(lhs: expr, rhs: NULL)
    ND_IFSTMT, // ND_IFSTMT(lhs: ND_IFBRANCH(lhs:if-cond, rhs: if-stmt), rhs: else-stmt)
    ND_IFBRANCH, // ND_IFBRANCH(lhs:if-cond, rhs: if-stmt)
    ND_WHILESTMT, // ND_WHILESTMT(lhs:cond, rhs: stmt)
    ND_BLOCK, // ND_BLOCK(lhs:stmt, rhs: ND_BLOCK(lhs:stmt, rhs: NULL))
    ND_CALLFUNC, // ND_CALLFUNC(lhs: NULL or ND_CALLARG, rhs: NULL)
    ND_CALLARG, // ND_CALLARG(lhs:expr, rhs: ND_CALLARG(lhs:expr, rhs: NULL))
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
    Token *tok;
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

extern LVar *locals;
extern Node *code[100];