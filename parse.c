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
LVar *locals;
Node *code[100];

bool consume_token_kind(TokenKind kind) {
    if (token->kind != kind) {
        return false;
    }
    token = token->next;
    return true;
}

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

LVar *find_lvar(Token *tok) {
    if (tok->kind != TK_IDENT) {
        error("Token TK_IDENT expected, but got %d.", tok->kind);
    }
    for (LVar *var = locals; var; var=var->next) {
        if (var->len == tok->len && !memcmp(var->name, tok->str, var->len)) {
            return var;
        }
    }
    return NULL;
}

LVar *push_lvar(Token *tok, LVar *head) {
    LVar *var = calloc(1, sizeof(LVar));
    var->name = tok->str;
    var->len = tok->len;
    var->offset = head ? head->offset + 8: 8;
    var->next = head;
    return var;
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
    Node *node;
    if (consume_token_kind(TK_RETURN)) {
        node = new_node(ND_RETURN, parse_expr(), NULL);
        expect_punct(";");
    } else if (consume_token_kind(TK_IF)) {
        expect_punct("(");
        Node *cond = parse_expr();
        expect_punct(")");
        Node *stmt = parse_stmt();
        node = new_node(ND_IFSTMT, new_node(ND_IFBRANCH, cond, stmt), NULL);
        if (consume_token_kind(TK_ELSE)) {
            node->rhs = parse_stmt();
        }
    } else if (consume_token_kind(TK_WHILE)) {
        expect_punct("(");
        Node *cond = parse_expr();
        expect_punct(")");
        Node *stmt = parse_stmt();
        node = new_node(ND_WHILESTMT, cond, stmt);
    } else {
        node = parse_expr();
        expect_punct(";");
    }

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
        LVar *lvar = find_lvar(tok);
        if (!lvar) {
            locals = push_lvar(tok, locals);
            lvar = locals;
        }
        Node *node = new_node(ND_LVAR, NULL, NULL);
        node->offset = lvar->offset;
        return node;
    }

    return new_node_num(expect_number());
}