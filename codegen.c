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

int mangle_counter = 0;

int new_mangle_number() {
    return ++mangle_counter;
}

void codegen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        error("ND_LVAR expected.");
    }
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

void codegen(Node *node) {
    if (node == NULL) {
        error("got NULL");
    }
    switch (node->kind) {
        case ND_NUM:
            printf("    push %d\n", node->val);
            return;
        case ND_LVAR:
            codegen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:
            //printf("# codegen ND_ASSIGN called.");
            codegen_lval(node->lhs);
            codegen(node->rhs);
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
        case ND_RETURN:
            codegen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
        case ND_IFSTMT:{
            int else_label = new_mangle_number();
            int end_label = new_mangle_number();
            codegen(node->lhs->lhs); // ifbranch -> cond
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lelse%010d\n", else_label);
            codegen(node->lhs->rhs); // ifbranch -> stmt
            printf("    jmp .Lend%010d\n", end_label);
            printf(".Lelse%010d:\n", else_label);
            if (node->rhs) {
                codegen(node->rhs); // else -> stmt
            }
            printf(".Lend%010d:\n", end_label);
            return;}
        case ND_WHILESTMT:{
            int begin_label = new_mangle_number();
            int end_label = new_mangle_number();
            printf(".Lbegin%010d:\n", begin_label);
            codegen(node->lhs);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%010d\n", end_label);
            codegen(node->rhs);
            printf("    jmp .Lbegin%010d\n", begin_label);
            printf(".Lend%010d:\n", end_label);
            return;}
        case ND_BLOCK: {
            codegen(node->lhs);
            if (node->rhs) {
                printf("    pop rax\n");
                codegen(node->rhs);
            }
            return;
        }
    }

    codegen(node->lhs);
    codegen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NEQ:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LEQ:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_GT:
            printf("    cmp rdi, rax\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_GEQ:
            printf("    cmp rdi, rax\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
        default:
            error("unreachable");
    }

    printf("    push rax\n");
}

void codegen_main() {
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    for (int i=0; code[i]; i++) {
        codegen(code[i]);
        printf("    pop rax\n");
    }

    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");

    return;
}