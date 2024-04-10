#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokenize.h"
#include "parse.h"
#include "codegen.h"
#include "debugprint.h"

typedef enum {
    OPT_TOKENIZE,
    OPT_PARSE,
    OPT_COMPILE,
    OPT_NONE,
} Option;

int main(int argc, char **argv) {
    Option opt = OPT_NONE;
    if (argc != 3) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    if (strcmp(argv[1], "tokenize") == 0) {
        opt = OPT_TOKENIZE;
    } else if (strcmp(argv[1], "parse") == 0) {
        opt = OPT_PARSE;
    } else if (strcmp(argv[1], "compile") == 0) {
        opt = OPT_COMPILE;
    } else {
        fprintf(stderr, "オプションが正しくありません\n");
        return 1;
    }

    Token *tok = tokenize(argv[2]);
    if (opt == OPT_TOKENIZE) {
        print_tokens(tok);
        return 0;
    }

    parse_program(tok);
    if (opt == OPT_PARSE) {
        for (int i=0; code[i]; i++) {
            print_nodetree(code[i]);
        }
        return 0;
    }

    codegen_main();

    return 0;
}