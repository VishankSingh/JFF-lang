/**
 * File Name: main.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "include/lexer.h"
#include "include/parser.h"


int main(void) {
    char *filename = "/home/vis/Desk/codes/compilers_1/part_2/examples/e1.jff";
    lexer_t *lexer = init_lexer(filename);
    if (!lexer) {
        fprintf(stderr, "Failed to initialize lexer\n");
        return 1;
    }
    token_t *token;
    while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
        lexer_apend_token(lexer, token);
        // print_token(token);
    }
    lexer_apend_token(lexer, token);
    // print_token(token);

    token_t** tokens = lexer->tokens;
    
    // for(size_t i = 0; i < lexer->token_count; i++) {
    //     print_token(tokens[i]);
    // }

    parser_t *parser = init_parser(lexer);
    if (!parser) {
        fprintf(stderr, "Failed to initialize parser\n");
        free_lexer(lexer);
        return 1;
    }
    parser_parse_program(parser);
    
    free_lexer(lexer);
    return 0;
}