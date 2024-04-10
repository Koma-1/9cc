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
Node *code[100];

void expect_punct(char *op) {
    if (token->kind != TK_RESERVED) {
        error("TK_RESERVED token \"%s\" expected, but got %d('%c')", op, token->kind, token->str[0]);
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

Token *consume_ident() {
    if (token->kind == TK_IDENT) {
        Token *tok = token;
        token = token->next;
        return tok;
    }
    return NULL;
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


Node *parse_stmt();
Node *parse_expr();
Node *parse_assign();
Node *parse_equality();
Node *parse_relational();
Node *parse_add();
Node *parse_term();
Node *parse_unary();
Node *parse_factor();

Node *parse(Token *tok) {
    token = tok;
    return parse_expr();
}

void parse_program(Token *tok) {
    token = tok;
    int i = 0;
    while (!at_eof()) {
        if (i >= 100-1) {
            error("too long program");
        }
        code[i] = parse_stmt();
        i++;
        code[i] = NULL;
    }
}

Node *parse_stmt() {
    Node *node = parse_expr();
    expect_punct(";");
    return node;
}

Node *parse_expr() {
    return parse_assign();
}

Node *parse_assign() {
    Node *node = parse_equality();
    if (consume_punct("=")) {
        node = new_node(ND_ASSIGN, node, parse_equality());
    }
    return node;
}

Node *parse_equality() {
    Node *node = parse_relational();

    for (;;) {
        if (consume_punct("==")) {
            node = new_node(ND_EQ, node, parse_relational());
        } else if (consume_punct("!=")) {
            node = new_node(ND_NEQ, node, parse_relational());
        } else {
            return node;
        }
    }
}

Node *parse_relational() {
    Node *node = parse_add();

    for (;;) {
        if (consume_punct("<")) {
            node = new_node(ND_LT, node, parse_add());
        } else if (consume_punct("<=")) {
            node = new_node(ND_LEQ, node, parse_add());
        } else if (consume_punct(">")) {
            node = new_node(ND_GT, node, parse_add());
        } else if (consume_punct(">=")) {
            node = new_node(ND_GEQ, node, parse_add());
        } else {
            return node;
        }
    }
}

Node *parse_add() {
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
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = new_node(ND_LVAR, NULL, NULL);
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }

    return new_node_num(expect_number());
}