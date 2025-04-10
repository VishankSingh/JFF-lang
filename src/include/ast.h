/**
 * File Name: ast.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

typedef enum {
    // Expressions
    AST_EXPR_LITERAL_INT,
    AST_EXPR_LITERAL_FLOAT,
    AST_EXPR_LITERAL_STRING,
    AST_EXPR_IDENTIFIER,
    AST_EXPR_BINARY,
    AST_EXPR_UNARY,
    AST_EXPR_ASSIGNMENT,
    AST_EXPR_CALL,
    AST_EXPR_ARG_LIST,

    // Statements
    AST_STMT_VAR_DECL,
    AST_STMT_ASSIGN,
    AST_STMT_RETURN,
    AST_STMT_PRINT,
    AST_STMT_BREAK,
    AST_STMT_CONTINUE,
    AST_STMT_IF,
    AST_STMT_WHILE,
    AST_STMT_FOR,
    AST_STMT_EXPR,
    AST_STMT_BLOCK,

    // Declarations
    AST_DECL_PARAM,
    AST_DECL_FUNCTION,
    AST_DECL_GLOBAL_VAR
} ast_node_type_t;

typedef enum {
    AST_DATA_TYPE_INT,
    AST_DATA_TYPE_FLOAT,
    AST_DATA_TYPE_BOOL,
    AST_DATA_TYPE_STRING,
    AST_DATA_TYPE_VOID,
} type_kind_t;

typedef struct AST_NODE_STRUCT ast_node_t;
typedef struct ast_expr_node_struct ast_expr_node_t;
typedef struct ast_stmt_node_struct ast_stmt_node_t;
//--------------------------------------- Expression Node -----------------------------------------

typedef enum {
    EXPR_LITERAL_INT,
    EXPR_LITERAL_FLOAT,
    EXPR_LITERAL_STRING,
    EXPR_IDENTIFIER,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_ASSIGNMENT,
    EXPR_CALL
} expr_kind_t;

typedef struct expr_literal_int_struct {
    int value;
} expr_literal_int_t;

typedef struct expr_literal_float_struct {
    float value;
} expr_literal_float_t;

typedef struct expr_literal_string_struct {
    char *value;
} expr_literal_string_t;

typedef struct expr_identifier_struct {
    char *name;
} expr_identifier_t;

typedef struct expr_binary_struct {
    ast_expr_node_t *left;
    token_type_t operator;
    ast_expr_node_t *right;
} expr_binary_t;

typedef struct expr_unary_struct {
    token_type_t operator;
    ast_expr_node_t *operand;
} expr_unary_t;

typedef struct expr_assignment_struct {
    char *name;
    ast_expr_node_t *value;
} expr_assignment_t;

typedef struct expr_call_struct {
    char *name;
    expr_arg_list_t *args;
} expr_call_t;

typedef struct expr_arg_list_struct {
    ast_node_t **args;
    size_t arg_count;
} expr_arg_list_t;

struct ast_expr_node_struct {
    expr_kind_t kind;
    union {
        expr_literal_int_t literal_int;
        expr_literal_float_t literal_float;
        expr_literal_string_t literal_string;
        expr_identifier_t identifier;
        expr_binary_t binary;
        expr_unary_t unary;
        expr_assignment_t assignment;
        expr_call_t call;
        expr_arg_list_t arg_list;
    } data;

    size_t line;
    size_t column;
};

//--------------------------------------- Statement Node -----------------------------------------
typedef enum {
    STMT_VAR_DECL,
    STMT_ASSIGN,
    STMT_RETURN,
    STMT_PRINT,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_EXPR
} stmt_kind_t;

typedef struct stmt_var_decl_struct {
    char *name;
    type_kind_t type;
    ast_expr_node_t *initializer;
} stmt_var_decl_t;

typedef struct stmt_assign_struct {
    char *name;
    ast_expr_node_t *value;
} stmt_assign_t;

typedef struct stmt_return_struct {
    ast_expr_node_t *value;
} stmt_return_t;

typedef struct stmt_print_struct {
    expr_arg_list_t *args;
} stmt_print_t;

typedef struct stmt_break_struct {
    // No additional fields needed
} stmt_break_t;

typedef struct stmt_continue_struct {
    // No additional fields needed
} stmt_continue_t;

typedef struct stmt_if_struct {
    ast_expr_node_t *if_condition;
    ast_stmt_node_t **if_branch;
    size_t if_branch_size;

    ast_expr_node_t **elif_conditions;
    ast_stmt_node_t ***elif_branches;
    size_t elif_count;
    size_t *elif_branch_size;

    ast_stmt_node_t **else_branch;
    size_t else_branch_size;
} stmt_if_t;

typedef struct stmt_while_struct {
    ast_expr_node_t *condition;
    ast_stmt_node_t **body;
    size_t body_size;
} stmt_while_t;

typedef struct for_init_node_struct {
    union {
        stmt_var_decl_t var_decl;
        stmt_assign_t assign;
    } data;
} for_init_node_t;

typedef struct stmt_for_struct {
    for_init_node_t *init;
    ast_expr_node_t *condition;
    stmt_assign_t increment;
    ast_stmt_node_t **body;
    size_t body_size;
} stmt_for_t;

typedef struct stmt_expr_struct {
    ast_expr_node_t *expression;
} stmt_expr_t;

typedef struct stmt_block_struct {
    ast_stmt_node_t **statements;
    size_t statement_count;
} stmt_block_t;

struct ast_stmt_node_struct {
    stmt_kind_t kind;
    union {
        stmt_var_decl_t var_decl;
        stmt_assign_t assign;
        stmt_return_t return_stmt;
        stmt_print_t print_stmt;
        stmt_break_t break_stmt;
        stmt_continue_t continue_stmt;
        stmt_if_t if_stmt;
        stmt_while_t while_stmt;
        stmt_for_t for_stmt;
        stmt_expr_t expr_stmt;
        stmt_block_t block_stmt;
    } data;

    size_t line;
    size_t column;
};

//--------------------------------------- Declaration Node --------------------------------------
typedef struct param_struct {
    char *name;
    type_kind_t type;
} param_t;

typedef struct decl_function_struct {
    char *name;
    type_kind_t return_type;
    ast_node_t **params;
    size_t param_count;
    ast_node_t **body;
    size_t body_count;
} decl_function_t;

typedef struct decl_global_var_struct {
    char *name;
    type_kind_t type;
    ast_node_t *initializer;
} decl_global_var_t;

//--------------------------------------- AST Node ----------------------------------------------
struct AST_NODE_STRUCT {
    ast_node_type_t type;
    union {
        expr_literal_int_t literal_int;
        expr_literal_float_t literal_float;
        expr_literal_string_t literal_string;
        expr_identifier_t identifier;
        expr_binary_t binary;
        expr_unary_t unary;
        expr_assignment_t assignment;
        expr_call_t call;
        expr_arg_list_t arg_list;

        stmt_var_decl_t var_decl;
        stmt_assign_t assign;
        stmt_return_t return_stmt;
        stmt_print_t print_stmt;
        stmt_break_t break_stmt;
        stmt_continue_t continue_stmt;
        stmt_if_t if_stmt;
        stmt_while_t while_stmt;
        stmt_for_t for_stmt;
        stmt_expr_t expr_stmt;
        stmt_block_t block_stmt;

        param_t param;
        decl_function_t function_decl;
        decl_global_var_t global_var_decl;

    } data;

    size_t line;
    size_t column;
};

typedef struct AST_STRUCT {
    ast_node_t **nodes;
    size_t node_count;
} ast_t;
//--------------------------------------- Function Prototypes -----------------------------------

void free_ast_node(ast_node_t *node);
void free_ast(ast_t *ast);
ast_t *init_ast();
void ast_add_node(ast_t *ast, ast_node_t *node);

//--------------------------------------- AST Node Initializers ---------------------------------
//-------------------- Expression Node Initializers
ast_node_t *init_expr_literal_int(int value, size_t line, size_t column);
ast_node_t *init_expr_literal_float(float value, size_t line, size_t column);
ast_node_t *init_expr_literal_string(const char * const value, size_t line, size_t column);
ast_node_t *init_expr_identifier(const char *name, size_t line, size_t column);
ast_node_t *init_expr_binary(ast_node_t *left, token_type_t operator, ast_node_t *right, size_t line, size_t column);
ast_node_t *init_expr_unary(token_type_t operator, ast_node_t *operand, size_t line, size_t column);
ast_node_t *init_expr_assignment(const char *name, ast_node_t *value, size_t line, size_t column);
ast_node_t *init_expr_call(const char *name, ast_node_t **args, size_t arg_count, size_t line, size_t column);
ast_node_t *init_expr_arg_list(ast_node_t **args, size_t arg_count, size_t line, size_t column);

//-------------------- Statement Node Initializers
ast_node_t *init_stmt_var_decl(const char *name, type_kind_t type, ast_node_t *initializer, size_t line, size_t column);
ast_node_t *init_stmt_assign(const char *name, ast_node_t *value, size_t line, size_t column);
ast_node_t *init_stmt_return(ast_node_t *value, size_t line, size_t column);
ast_node_t *init_stmt_print(ast_node_t **args, size_t arg_count, size_t line, size_t column);
ast_node_t *init_stmt_break(size_t line, size_t column);
ast_node_t *init_stmt_continue(size_t line, size_t column);
ast_node_t *init_stmt_if(ast_node_t *condition, ast_node_t **then_branch, size_t then_branch_count,
                          ast_node_t **elif_conditions, ast_node_t ***elif_branches, size_t elif_count,
                          ast_node_t **else_branch, size_t else_branch_count, size_t line, size_t column);
ast_node_t *init_stmt_while(ast_node_t *condition, ast_node_t **body, size_t body_count, size_t line, size_t column);
ast_node_t *init_stmt_for(ast_node_t *init, ast_node_t *condition, ast_node_t *increment,
                            ast_node_t **body, size_t body_count, size_t line, size_t column);
ast_node_t *init_stmt_expr(ast_node_t *expression, size_t line, size_t column);
ast_node_t *init_stmt_block(ast_node_t **statements, size_t statement_count, size_t line, size_t column);

//-------------------- Declaration Node Initializers
ast_node_t *init_decl_param(const char *name, type_kind_t type, size_t line, size_t column);
ast_node_t *init_decl_function(const char *name, type_kind_t return_type, ast_node_t **params,
                                 size_t param_count, ast_node_t **body, size_t body_count, size_t line, size_t column);
ast_node_t *init_decl_global_var(const char *name, type_kind_t type, ast_node_t *initializer,
                                    size_t line, size_t column);


void print_ast(ast_t *ast);
void print_ast_node(ast_node_t *node, int indent_level);


#endif // AST_H