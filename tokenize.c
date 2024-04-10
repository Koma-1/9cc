#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokenize.h"

bool startswith(char *str, char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool is_alnum(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
}

int get_alnum_str(char *c, char **endptr) {
    char *tmp = c;
    int len = 0;
    while (is_alnum(*tmp)) {
        len++;
        tmp++;
    }
    *endptr = tmp;
    return len;
}

Token *push_token(TokenKind kind, Token *tail, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    if (tok == NULL) {
        error("calloc error");
    }
    tok->kind = kind;
    tok->next = NULL;
    tok->str = str;
    tok->len = len;
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

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
            current_token = push_token(TK_RESERVED, current_token, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>=;", *p)) {
            current_token = push_token(TK_RESERVED, current_token, p, 1);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            current_token = push_token(TK_NUM, current_token, p, 0);
            current_token->val = strtol(p, &p, 10);
            continue;
        }

        if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || (*p == '_')) {
            char *endp;
            int len = get_alnum_str(p, &endp);
            if (len == 6 && !strncmp(p, "return", len)) {
                current_token = push_token(TK_RETURN, current_token, p, len);
            } else if (len == 2 && !strncmp(p, "if", len)) {
                current_token = push_token(TK_IF, current_token, p, len);
            } else if (len == 4 && !strncmp(p, "else", len)) {
                current_token = push_token(TK_ELSE, current_token, p, len);
            } else {
                current_token = push_token(TK_IDENT, current_token, p, len);
            }
            p = endp;
            continue;
        }

        error("tokenize");
    }

    current_token = push_token(TK_EOF, current_token, p, 0);
    return head.next;
}

