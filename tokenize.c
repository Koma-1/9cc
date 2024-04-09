#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokenize.h"

Token *push_token(TokenKind kind, Token *tail, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    if (tok == NULL) {
        error("calloc error");
    }
    tok->kind = kind;
    tok->next = NULL;
    tok->str = str;
    tail->next = tok;

    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *current_token = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strchr("+-*/()", *p)) {
            current_token = push_token(TK_RESERVED, current_token, p);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            current_token = push_token(TK_NUM, current_token, p);
            current_token->val = strtol(p, &p, 10);
            continue;
        }

        error("tokenize");
    }

    current_token = push_token(TK_EOF, current_token, p);
    return head.next;
}

