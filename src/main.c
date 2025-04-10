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

    
    // token_t** tokens = lexer->tokens;
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

    ast_node_t *first_node = parser->ast->nodes[0];
    if (first_node) {
        printf("First node type: %d\n", first_node->type);
        printf("First node line: %zu\n", first_node->line);
        printf("First node column: %zu\n", first_node->column);
    } else {
        printf("No nodes in AST\n");
    }
    
    free_lexer(lexer);
    return 0;
}