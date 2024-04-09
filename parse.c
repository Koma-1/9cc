#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokenize.h"
#include "parse.h"


Token *token;

void expect_punct(char *op) {
    if (token->kind != TK_RESERVED) {
        error("TK_RESERVED token expected, but got %d", token->kind);
        if (strlen(op) != token->len || !startswith(token->str, op)) {
            error("operator '%c' expected", op);
        }
    }
    token = token->next;
}

bool consume_punct(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || !startswith(token->str, op)) {
        return false;
    }
    token = token->next;
    return true;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error("TK_NUM token expected, but got %d", token->kind);
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}


Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = new_node(ND_NUM, NULL, NULL);
    node->val = val;
    return node;
}


Node *parse_expr();
Node *parse_term();
Node *parse_unary();
Node *parse_factor();

Node *parse(Token *tok) {
    token = tok;
    return parse_expr();
}

Node *parse_expr() {
    Node *node = parse_term();

    for (;;) {
        if (consume_punct("+")) {
            node = new_node(ND_ADD, node, parse_term());
        } else if (consume_punct("-")) {
            node = new_node(ND_SUB, node, parse_term());
        } else {
            return node;
        }
    }
}

Node *parse_term() {
    Node *node = parse_unary();

    for (;;) {
        if (consume_punct("*")) {
            node = new_node(ND_MUL, node, parse_unary());
        } else if (consume_punct("/")) {
            node = new_node(ND_DIV, node, parse_unary());
        } else {
            return node;
        }
    }
}

Node *parse_unary() {
    if (consume_punct("+")) {
        return parse_factor();
    }
    if (consume_punct("-")) {
        return new_node(ND_SUB, new_node_num(0), parse_factor());
    }
    return parse_factor();
}

Node *parse_factor() {
    if (consume_punct("(")) {
        Node *node = parse_expr();
        expect_punct(")");
        return node;
    } else {
        return new_node_num(expect_number());
    }
}