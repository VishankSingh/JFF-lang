/**
 * File Name: ast.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include "include/ast.h"
#include "include/parser.h"
#include "include/lexer.h"


void free_ast_node(ast_node_t *node) {

}

void free_ast(ast_t *ast) {

}

ast_t *init_ast() {
    ast_t *ast = malloc(sizeof(ast_t));
    if (!ast) {
        fprintf(stderr, "Failed to allocate memory for AST\n");
        return NULL;
    }
    ast->nodes = NULL;
    ast->node_count = 0;
    return ast;
}

//-------------------- Expression Node Initializers
ast_node_t *init_expr_literal_int(int value, size_t line, size_t column) {

}

ast_node_t *init_expr_literal_float(float value, size_t line, size_t column) {

}

ast_node_t *init_expr_literal_string(const char * const value, size_t line, size_t column) {

}

ast_node_t *init_expr_identifier(const char *name, size_t line, size_t column) {

}

ast_node_t *init_expr_binary(ast_node_t *left, token_type_t operator, ast_node_t *right, size_t line, size_t column) {

}

ast_node_t *init_expr_unary(token_type_t operator, ast_node_t *operand, size_t line, size_t column) {

}

ast_node_t *init_expr_assignment(const char *name, ast_node_t *value, size_t line, size_t column) {

}

ast_node_t *init_expr_call(const char *name, ast_node_t **args, size_t arg_count, size_t line, size_t column) {

}

ast_node_t *init_expr_arg_list(ast_node_t **args, size_t arg_count, size_t line, size_t column) {

}


//-------------------- Statement Node Initializers
ast_node_t *init_stmt_var_decl(const char *name, type_kind_t type, ast_node_t *initializer, size_t line, size_t column) {

}

ast_node_t *init_stmt_assign(const char *name, ast_node_t *value, size_t line, size_t column) {

}

ast_node_t *init_stmt_return(ast_node_t *value, size_t line, size_t column) {

}

ast_node_t *init_stmt_print(ast_node_t **args, size_t arg_count, size_t line, size_t column) {

}

ast_node_t *init_stmt_break(size_t line, size_t column) {

}

ast_node_t *init_stmt_continue(size_t line, size_t column) {

}

ast_node_t *init_stmt_if(ast_node_t *condition, ast_node_t **then_branch, size_t then_branch_count,
                          ast_node_t **elif_conditions, ast_node_t ***elif_branches, size_t elif_count,
                          ast_node_t **else_branch, size_t else_branch_count, size_t line, size_t column) {

}

ast_node_t *init_stmt_while(ast_node_t *condition, ast_node_t **body, size_t body_count, size_t line, size_t column) {

}

ast_node_t *init_stmt_for(ast_node_t *init, ast_node_t *condition, ast_node_t *increment,
                            ast_node_t **body, size_t body_count, size_t line, size_t column) {

}
ast_node_t *init_stmt_expr(ast_node_t *expression, size_t line, size_t column) {

}
ast_node_t *init_stmt_block(ast_node_t **statements, size_t statement_count, size_t line, size_t column) {

}

//-------------------- Declaration Node Initializers 
ast_node_t *init_decl_param(const char *name, type_kind_t type, size_t line, size_t column) {

}
ast_node_t *init_decl_function(const char *name, type_kind_t return_type, ast_node_t **params,
                                 size_t param_count, ast_node_t **body, size_t body_count, size_t line, size_t column) {

}
ast_node_t *init_decl_global_var(const char *name, type_kind_t type, ast_node_t *initializer,
                                    size_t line, size_t column) {

}
