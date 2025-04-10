/**
 * File Name: ast.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include "include/ast.h"
#include "include/parser.h"
#include "include/lexer.h"

#define _POSIX_C_SOURCE 200809L 
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


void free_ast_node(ast_node_t *node) {
    (void)node;
}

void free_ast(ast_t *ast) {
    (void)ast;
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

ast_node_t *init_ast_node(ast_node_category_t type, size_t line, size_t column) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for AST node\n");
        return NULL;
    }
    node->type = type;
    node->line = line;
    node->column = column;
    return node;
}

//-------------------- Expression Node Initializers ---------------------------------------------
ast_expr_node_t *init_expr_literal_int(int value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_LITERAL_INT;
    node->data.literal_int.value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_literal_float(float value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_LITERAL_FLOAT;
    node->data.literal_float.value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_literal_string(const char * const value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_LITERAL_STRING;
    node->data.literal_string.value = strdup(value);
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_identifier(const char *name, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_IDENTIFIER;
    node->data.identifier.name = strdup(name);
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_binary(ast_expr_node_t *left, ast_expr_node_t *right, token_type_t operator, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_BINARY;
    node->data.binary.left = left;
    node->data.binary.right = right;
    node->data.binary.operator = operator;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_unary(token_type_t operator, ast_expr_node_t *operand, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_UNARY;
    node->data.unary.operator = operator;
    node->data.unary.operand = operand;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_assignment(const char *name, ast_expr_node_t *value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_ASSIGNMENT;
    node->data.assignment.name = strdup(name);
    node->data.assignment.value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_call(const char *name, expr_arg_list_t *arg_list, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_CALL;
    node->data.call.name = strdup(name);
    node->data.call.args = arg_list;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_arg_list(ast_expr_node_t **args, size_t arg_count, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for expression node\n");
        return NULL;
    }
    node->type = EXPR_ARG_LIST;
    node->data.arg_list.args = args;
    node->data.arg_list.arg_count = arg_count;
    node->line = line;
    node->column = column;
    return node;
}


//-------------------- Statement Node Initializers ----------------------------------------------
ast_stmt_node_t *init_stmt_var_decl(const char *name, data_type_t type, ast_expr_node_t *initializer, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_VAR_DECL;
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.type = type;
    node->data.var_decl.initializer = initializer;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_assign(const char *name, ast_expr_node_t *value, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_ASSIGN;
    node->data.assign.name = strdup(name);
    node->data.assign.value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_return(ast_expr_node_t *value, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_RETURN;
    node->data.return_stmt.value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_print(expr_arg_list_t *args, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_PRINT;
    node->data.print_stmt.args = args;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_break(size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_BREAK;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_continue(size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_CONTINUE;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_if(ast_expr_node_t *if_condition, ast_stmt_node_t **if_branch, size_t if_branch_size,
                          ast_expr_node_t **elif_conditions, ast_stmt_node_t ***elif_branches, size_t elif_count,
                          size_t *elif_branch_size, ast_stmt_node_t **else_branch, size_t else_branch_size,
                          size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_IF;
    node->data.if_stmt.if_condition = if_condition;
    node->data.if_stmt.if_branch = if_branch;
    node->data.if_stmt.if_branch_size = if_branch_size;
    node->data.if_stmt.elif_conditions = elif_conditions;
    node->data.if_stmt.elif_branches = elif_branches;
    node->data.if_stmt.elif_count = elif_count;
    node->data.if_stmt.elif_branch_size = elif_branch_size;
    node->data.if_stmt.else_branch = else_branch;
    node->data.if_stmt.else_branch_size = else_branch_size;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_while(ast_expr_node_t *condition, ast_stmt_node_t **body, size_t body_size, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_WHILE;
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    node->data.while_stmt.body_size = body_size;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_for(for_init_kind_t init_kind, stmt_var_decl_t *var_decl, stmt_assign_t *assign,
                           ast_expr_node_t *condition, stmt_assign_t *increment, ast_stmt_node_t **body,
                           size_t body_size, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_FOR;
    node->data.for_stmt.init_kind = init_kind;
    if (init_kind == FOR_INIT_VAR_DECL) {
        node->data.for_stmt.init.var_decl = var_decl;
    } else if (init_kind == FOR_INIT_ASSIGN) {
        node->data.for_stmt.init.assign = assign;
    }
    node->data.for_stmt.condition = condition;
    node->data.for_stmt.increment = increment;
    node->data.for_stmt.body = body;
    node->data.for_stmt.body_size = body_size;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_expr(ast_expr_node_t *expression, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_EXPR;
    node->data.expr_stmt.expression = expression;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_block(ast_stmt_node_t **statements, size_t statement_count, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for statement node\n");
        return NULL;
    }
    node->type = STMT_BLOCK;
    node->data.block_stmt.statements = statements;
    node->data.block_stmt.statement_count = statement_count;
    node->line = line;
    node->column = column;
    return node;
}


//-------------------- Declaration Node Initializers -------------------------------------------
param_t *init_decl_param(const char *name, data_type_t type, size_t line, size_t column) {
    (void)line;
    (void)column;
    param_t *param = malloc(sizeof(param_t));
    if (!param) {
        fprintf(stderr, "Failed to allocate memory for parameter\n");
        return NULL;
    }
    param->name = strdup(name);
    param->type = type;
    return param;
}

ast_decl_node_t *init_decl_function(const char *name, data_type_t return_type, param_t **params,
                                 size_t param_count, ast_stmt_node_t **body, size_t body_count,
                                 size_t line, size_t column) {
    ast_decl_node_t *node = malloc(sizeof(ast_decl_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for declaration node\n");
        return NULL;
    }
    node->type = DECL_FUNCTION;
    node->data.function_decl.name = strdup(name);
    node->data.function_decl.return_type = return_type;
    node->data.function_decl.params = params;
    node->data.function_decl.param_count = param_count;
    node->data.function_decl.body = body;
    node->data.function_decl.body_count = body_count;
    node->line = line;
    node->column = column;
    return node;
}

ast_decl_node_t *init_decl_global_var(const char *name, data_type_t type, ast_expr_node_t *initializer,
                                 size_t line, size_t column) {
    ast_decl_node_t *node = malloc(sizeof(ast_decl_node_t));
    if (!node) {
        fprintf(stderr, "Failed to allocate memory for declaration node\n");
        return NULL;
    }
    node->type = DECL_GLOBAL_VAR;
    node->data.global_var_decl.name = strdup(name);
    node->data.global_var_decl.type = type;
    node->data.global_var_decl.initializer = initializer;
    node->line = line;
    node->column = column;
    return node;
}

void print_ast(ast_t *ast) {
    for (size_t i = 0; i < ast->node_count; i++) {
        ast_node_t *node = ast->nodes[i];
        if (node->type == AST_NODE_TYPE_EXPR) {
            // Print expression node
        } else if (node->type == AST_NODE_TYPE_STMT) {
            // Print statement node
        } else if (node->type == AST_NODE_TYPE_DECL) {
            // Print declaration node
        }
    }
}