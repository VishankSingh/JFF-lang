/**
 * File Name: main.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "include/lexer.h"
#include "include/parser.h"


int main(void) {
    char *filename = "/home/vis/Desk/codes/compilers_1/part_2/examples/e1.jff";
    lexer_t *lexer = init_lexer(filename);
    token_t *token;
    while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
        lexer_append_token(lexer, token);
        // print_token(token);
    }
    lexer_append_token(lexer, token); // EOF token
    // print_token(token);
    
    // token_t** tokens = lexer->tokens;
    // for(size_t i = 0; i < lexer->token_count; i++) {
    //     print_token(tokens[i]);
    // }

    parser_t *parser = init_parser(lexer);
    parser_parse_program(parser);
    print_ast(parser->ast);
    
    free_lexer(lexer);
    free_parser(parser);
    return 0;
}