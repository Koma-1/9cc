#pragma once
#include <stdbool.h>

typedef enum {
    TK_RESERVED,
    TK_NUM,
    TK_IDENT,
    TK_RETURN,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val; // TK_NUMで使う
    char *str;
    int len; // TK_RESERVEDで使う
};

bool startswith(char *str, char *prefix);
Token *tokenize(char *p);