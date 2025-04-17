/**
 * File Name: parser.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "lexer.h"
#include "ast.h"

typedef struct PARSER_STRUCT {
    token_t **tokens;
    size_t token_count;
    size_t current_index;
    lexer_t *lexer;
    token_t *current;
    token_t *previous;
    ast_t *ast;
} parser_t;

parser_t *init_parser(lexer_t *lexer);
void free_parser(parser_t *parser);

void parser_advance(parser_t *parser);

bool parser_match(parser_t *parser, token_type_t type);

token_t *parser_peek_token(parser_t *parser, size_t dist);

void parser_expect_advance(parser_t *parser, token_type_t type);
void parser_expect(parser_t *parser, token_type_t type);

void parser_parse_program(parser_t *parser);
ast_node_t *parser_parse_declaration(parser_t *parser);
ast_decl_node_t *parser_parse_function_decl(parser_t *parser);
void parser_parse_param_list(parser_t *parser);
void parser_parse_param(parser_t *parser);
data_type_t parser_parse_type(parser_t *parser);
ast_stmt_node_t *parser_parse_var_decl(parser_t *parser);
void parser_parse_statement(parser_t *parser);
void parser_parse_assignment(parser_t *parser);
void parser_parse_print_statement(parser_t *parser);
void parser_parse_return_statement(parser_t *parser);
void parser_parse_if_statement(parser_t *parser);
void parser_parse_for_statement(parser_t *parser);
void parser_parse_while_statement(parser_t *parser);
ast_expr_node_t *parser_parse_expression(parser_t *parser);
ast_expr_node_t *parser_parse_logical_or(parser_t *parser);
ast_expr_node_t *parser_parse_logical_and(parser_t *parser);
ast_expr_node_t *parser_parse_equality(parser_t *parser);
ast_expr_node_t *parser_parse_comparision(parser_t *parser);
ast_expr_node_t *parser_parse_term(parser_t *parser);
ast_expr_node_t *parser_parse_factor(parser_t *parser);
ast_expr_node_t *parser_parse_unary(parser_t *parser);
ast_expr_node_t *parser_parse_primary(parser_t *parser);
expr_arg_list_t *parser_parse_arg_list(parser_t *parser);

#endif // PARSER_H