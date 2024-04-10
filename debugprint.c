#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokenize.h"
#include "parse.h"
#include "codegen.h"


void print_tokens(Token *tok) {
    Token *current_token = tok;

    while (current_token) {
        switch (current_token->kind) {
            case TK_RESERVED:
                printf("%.*s\tTK_RESERVED\n", current_token->len, current_token->str);
                break;
            case TK_NUM:
                printf("%d\tTK_NUM\n", current_token->val);
                break;
            case TK_IDENT:
                printf("%.*s\tTK_IDENT\n", current_token->len, current_token->str);
                break;
            case TK_EOF:
                printf("TK_EOF\n");
                break;
        }
        current_token = current_token->next;
    }
}

void print_indent(int n) {
    for (int i=0; i<n; i++) {
        printf("    ");
    }
    return;
}

void print_nodes(Node *node, int indent) {
    print_indent(indent);
    if (node->kind == ND_NUM) {
        printf("ND_NUM{%d}", node->val);
    } else if (node->kind == ND_LVAR) {
        printf("ND_LVAR{%d}", node->offset);
    } else {
        switch (node->kind) {
            case ND_ADD:
                printf("ND_ADD");
                break;
            case ND_SUB:
                printf("ND_SUB");
                break;
            case ND_MUL:
                printf("ND_MUL");
                break;
            case ND_DIV:
                printf("ND_DIV");
                break;
            case ND_EQ:
                printf("ND_EQ");
                break;
            case ND_NEQ:
                printf("ND_NEQ");
                break;
            case ND_LT:
                printf("ND_LT");
                break;
            case ND_LEQ:
                printf("ND_LEQ");
                break;
            case ND_GT:
                printf("ND_GT");
                break;
            case ND_GEQ:
                printf("ND_GEQ");
                break;
            case ND_ASSIGN:
                printf("ND_ASSIGN");
                break;
            default:
                error("unreachable");
        }
        printf("{\n");
        print_nodes(node->lhs, indent+1);
        printf(",\n");
        print_nodes(node->rhs, indent+1);
        putchar('\n');
        print_indent(indent);
        putchar('}');
    }
}

void print_nodetree(Node *node) {
    print_nodes(node, 0);
    putchar('\n');
}