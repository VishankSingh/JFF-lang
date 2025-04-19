/**
 * File Name: ast.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#define _POSIX_C_SOURCE 200809L 

#include <stddef.h>
#include <string.h>

#include "include/ast.h"
#include "include/lexer.h"
#include "include/utils.h"

char *data_type_to_string(data_type_t type) {
    switch (type) {
        case DATA_TYPE_INT: return "int";
        case DATA_TYPE_FLOAT: return "float";
        case DATA_TYPE_BOOL: return "bool";
        case DATA_TYPE_STRING: return "string";
        case DATA_TYPE_VOID: return "void";
        default: return "unknown";
    }
}

void free_expr_node(ast_expr_node_t *expr) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_LITERAL_INT: {
            free(expr->data.literal_int);
            break;
        }
        case EXPR_LITERAL_FLOAT: {
            free(expr->data.literal_float);
            break;
        }
        case EXPR_LITERAL_STRING: {
            free(expr->data.literal_string->value);
            free(expr->data.literal_string);
            break;
        }
        case EXPR_IDENTIFIER: {
            free(expr->data.identifier->name);
            free(expr->data.identifier);
            break;
        }
        case EXPR_ASSIGNMENT: {
            free(expr->data.assignment->name);
            free_expr_node(expr->data.assignment->value);
            free(expr->data.assignment);
            break;
        }
        case EXPR_BINARY: {
            free_expr_node(expr->data.binary->left);
            free_expr_node(expr->data.binary->right);
            free(expr->data.binary);
            break;
        }
        case EXPR_UNARY: {
            free_expr_node(expr->data.unary->operand);
            free(expr->data.unary);
            break;
        }
        case EXPR_CALL: {
            free(expr->data.call->name);
            for (size_t i = 0; i < expr->data.call->args->arg_count; ++i) {
                free_expr_node(expr->data.call->args->args[i]);
            }
            free(expr->data.call->args->args);
            free(expr->data.call->args);
            free(expr->data.call);
            break;
        }
        case EXPR_ARG_LIST: {
            for (size_t i = 0; i < expr->data.arg_list->arg_count; ++i) {
                free_expr_node(expr->data.arg_list->args[i]);
            }
            free(expr->data.arg_list->args);
            free(expr->data.arg_list);
            break;
        }
    }

    free(expr);
}

void free_stmt_node(ast_stmt_node_t *stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_VAR_DECL: {
            free(stmt->data.var_decl->name);
            free_expr_node(stmt->data.var_decl->initializer);
            free(stmt->data.var_decl);
            break;
        }
        case STMT_ASSIGN: {
            free(stmt->data.assign->name);
            free_expr_node(stmt->data.assign->value);
            free(stmt->data.assign);
            break;
        }
        case STMT_RETURN: {
            free_expr_node(stmt->data.return_stmt->value);
            free(stmt->data.return_stmt);
            break;
        }
        case STMT_PRINT: {
            for (size_t i = 0; i < stmt->data.print_stmt->args->arg_count; ++i) {
                free_expr_node(stmt->data.print_stmt->args->args[i]);
            }
            free(stmt->data.print_stmt->args->args);
            free(stmt->data.print_stmt->args);
            free(stmt->data.print_stmt);
            break;
        }
        case STMT_BLOCK: {
            for (size_t i = 0; i < stmt->data.block_stmt->statement_count; ++i) {
                free_stmt_node(stmt->data.block_stmt->statements[i]);
            }
            free(stmt->data.block_stmt->statements);
            free(stmt->data.block_stmt);
            break;
        }
        case STMT_IF: {
            free_expr_node(stmt->data.if_stmt->if_condition);
            free_stmt_node(stmt->data.if_stmt->if_block);

            for (size_t i = 0; i < stmt->data.if_stmt->elif_blocks_count; ++i) {
                free_expr_node(stmt->data.if_stmt->elif_conditions[i]);
                free_stmt_node(stmt->data.if_stmt->elif_blocks[i]);
            }
            free(stmt->data.if_stmt->elif_conditions);
            free(stmt->data.if_stmt->elif_blocks);

            free_stmt_node(stmt->data.if_stmt->else_block);
            break;
        }
        case STMT_WHILE: {
            free_expr_node(stmt->data.while_stmt->condition);
            free_stmt_node(stmt->data.while_stmt->block);
            free(stmt->data.while_stmt);
            break;
        }
        // TODO: correct this
        case STMT_FOR: {
            if (stmt->data.for_stmt->init) {
                switch (stmt->data.for_stmt->init->kind) {
                    case FOR_INIT_VAR_DECL:
                        if (stmt->data.for_stmt->init->data.var_decl) {
                            free(stmt->data.for_stmt->init->data.var_decl->name);
                            free_expr_node(stmt->data.for_stmt->init->data.var_decl->initializer);
                            free(stmt->data.for_stmt->init->data.var_decl);
                        }
                        break;
                    case FOR_INIT_ASSIGN:
                        if (stmt->data.for_stmt->init->data.assign) {
                            free(stmt->data.for_stmt->init->data.assign->name);
                            free_expr_node(stmt->data.for_stmt->init->data.assign->value);
                            free(stmt->data.for_stmt->init->data.assign);
                        }
                        break;
                    case FOR_INIT_EXPR:
                        if (stmt->data.for_stmt->init->data.expr) {
                            free_expr_node(stmt->data.for_stmt->init->data.expr->expression);
                            free(stmt->data.for_stmt->init->data.expr);
                        }
                        break;
                    case FOR_INIT_NONE:
                        break;
                }
                free(stmt->data.for_stmt->init);
            }

            if (stmt->data.for_stmt->condition) {
                free_expr_node(stmt->data.for_stmt->condition);
            }

            if (stmt->data.for_stmt->increment) {
                free(stmt->data.for_stmt->increment->name);
                free_expr_node(stmt->data.for_stmt->increment->value);
                free(stmt->data.for_stmt->increment);
            }

            free_stmt_node(stmt->data.for_stmt->block);
            free(stmt->data.for_stmt);
            break;
        }
        case STMT_EXPR: {
            free_expr_node(stmt->data.expr_stmt->expression);
            free(stmt->data.expr_stmt);
            break;
        }
        case STMT_BREAK: {
            free(stmt->data.break_stmt);
        }
        case STMT_CONTINUE:
            // No dynamic memory to free for these types
            break;
    }

    free(stmt);
}

void free_decl_node(ast_decl_node_t *decl) {
    if (!decl) return;

    switch (decl->type) {
        case DECL_FUNCTION: {
            free(decl->data.function_decl->name);
            for (size_t i = 0; i < decl->data.function_decl->param_list->param_count; ++i) {
                free(decl->data.function_decl->param_list->params[i].name);
            }
            free(decl->data.function_decl->param_list->params);
            free(decl->data.function_decl->param_list);

            for (size_t i = 0; i < decl->data.function_decl->body_count; ++i) {
                free_stmt_node(decl->data.function_decl->body[i]);
            }
            free(decl->data.function_decl->body);
            free(decl->data.function_decl);
            break;
        }
    }

    free(decl);
}


void free_ast_node(ast_node_t *node) {
    if (!node) return;

    switch (node->type) {
        case AST_NODE_CATEGORY_EXPR:
            free_expr_node(node->data.expr_node);
            break;
        case AST_NODE_CATEGORY_STMT:
            free_stmt_node(node->data.stmt_node);
            break;
        case AST_NODE_CATEGORY_DECL:
            free_decl_node(node->data.decl_node);
            break;
    }
    free(node);
}

void free_ast(ast_t *ast) {
    if (!ast) return;
    
    for (size_t i = 0; i < ast->node_count; ++i) {
        free_ast_node(ast->nodes[i]);
    }
    free(ast->nodes);
    free(ast);
}

ast_t *init_ast(void) {
    ast_t *ast = malloc(sizeof(ast_t));
    CHECK_MEM_ALLOC_ERROR(ast);
    ast->node_count = 0;
    ast->nodes_capacity = 1;
    ast->nodes = malloc(ast->nodes_capacity * sizeof(ast_node_t *));
    CHECK_MEM_ALLOC_ERROR(ast->nodes);
    return ast;
}

ast_node_t *init_ast_node(ast_node_category_t type, size_t line, size_t column) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = type;
    node->line = line;
    node->column = column;
    switch (type) {
        case AST_NODE_CATEGORY_EXPR:
            node->data.expr_node = NULL;
            break;
        case AST_NODE_CATEGORY_STMT:
            node->data.stmt_node = NULL;
            break;
        case AST_NODE_CATEGORY_DECL:
            node->data.decl_node = NULL;
            break;
    }

    return node;
}

//-------------------- Expression Node Initializers ---------------------------------------------
ast_expr_node_t *init_expr_literal_int(int value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_LITERAL_INT;
    node->data.literal_int = malloc(sizeof(expr_literal_int_t));
    CHECK_MEM_ALLOC_ERROR(node->data.literal_int);
    node->data.literal_int->value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_literal_float(float value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_LITERAL_FLOAT;
    node->data.literal_float = malloc(sizeof(expr_literal_float_t));
    CHECK_MEM_ALLOC_ERROR(node->data.literal_float);
    node->data.literal_float->value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_literal_string(const char * const value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_LITERAL_STRING;
    node->data.literal_string = malloc(sizeof(expr_literal_string_t));
    CHECK_MEM_ALLOC_ERROR(node->data.literal_string);
    node->data.literal_string->value = strdup(value);
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_identifier(const char *name, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_IDENTIFIER;
    node->data.identifier = malloc(sizeof(expr_identifier_t));
    CHECK_MEM_ALLOC_ERROR(node->data.identifier);
    node->data.identifier->name = strdup(name);
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_binary(token_type_t operator, ast_expr_node_t *left, ast_expr_node_t *right, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_BINARY;
    node->data.binary = malloc(sizeof(expr_binary_t));
    CHECK_MEM_ALLOC_ERROR(node->data.binary);
    node->data.binary->left = left;
    node->data.binary->right = right;
    node->data.binary->operator = operator;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_unary(token_type_t operator, ast_expr_node_t *operand, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_UNARY;
    node->data.unary = malloc(sizeof(expr_unary_t));
    CHECK_MEM_ALLOC_ERROR(node->data.unary);
    node->data.unary->operator = operator;
    node->data.unary->operand = operand;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_assignment(const char *name, ast_expr_node_t *value, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_ASSIGNMENT;
    node->data.assignment = malloc(sizeof(expr_assignment_t));
    CHECK_MEM_ALLOC_ERROR(node->data.assignment);
    node->data.assignment->name = strdup(name);
    node->data.assignment->value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_call(const char *name, expr_arg_list_t *arg_list, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_CALL;
    node->data.call = malloc(sizeof(expr_call_t));
    CHECK_MEM_ALLOC_ERROR(node->data.call);
    node->data.call->name = strdup(name);
    node->data.call->args = arg_list;
    node->line = line;
    node->column = column;
    return node;
}

ast_expr_node_t *init_expr_arg_list(ast_expr_node_t **args, size_t arg_count, size_t line, size_t column) {
    ast_expr_node_t *node = malloc(sizeof(ast_expr_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = EXPR_ARG_LIST;
    node->data.arg_list = malloc(sizeof(expr_arg_list_t));
    CHECK_MEM_ALLOC_ERROR(node->data.arg_list);
    node->data.arg_list->args = args;
    node->data.arg_list->arg_count = arg_count;
    node->line = line;
    node->column = column;
    return node;
}


//-------------------- Statement Node Initializers ----------------------------------------------
ast_stmt_node_t *init_stmt_var_decl(const char *name, data_type_t type, ast_expr_node_t *initializer, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_VAR_DECL;
    node->data.var_decl = malloc(sizeof(stmt_var_decl_t));
    CHECK_MEM_ALLOC_ERROR(node->data.var_decl);
    node->data.var_decl->name = strdup(name);
    node->data.var_decl->type = type;
    node->data.var_decl->initializer = initializer;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_assign(const char *name, ast_expr_node_t *value, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_ASSIGN;
    node->data.assign = malloc(sizeof(stmt_assign_t));
    CHECK_MEM_ALLOC_ERROR(node->data.assign);
    node->data.assign->name = strdup(name);
    node->data.assign->value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_return(ast_expr_node_t *value, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_RETURN;
    node->data.return_stmt = malloc(sizeof(stmt_return_t));
    CHECK_MEM_ALLOC_ERROR(node->data.return_stmt);
    node->data.return_stmt->value = value;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_print(expr_arg_list_t *args, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_PRINT;
    node->data.print_stmt = malloc(sizeof(stmt_print_t));
    CHECK_MEM_ALLOC_ERROR(node->data.print_stmt);
    node->data.print_stmt->args = args;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_break(size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_BREAK;
    node->data.break_stmt = malloc(sizeof(stmt_break_t));
    CHECK_MEM_ALLOC_ERROR(node->data.break_stmt);
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_continue(size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_CONTINUE;
    node->data.continue_stmt = malloc(sizeof(stmt_continue_t));
    CHECK_MEM_ALLOC_ERROR(node->data.continue_stmt);
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_if(ast_expr_node_t *if_condition, ast_stmt_node_t *if_block, 
                              ast_expr_node_t **elif_conditions, size_t elif_blocks_count, 
                              ast_stmt_node_t **elif_blocks, ast_stmt_node_t *else_block,
                              size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_IF;
    node->data.if_stmt = malloc(sizeof(stmt_if_t));
    CHECK_MEM_ALLOC_ERROR(node->data.if_stmt);
    node->data.if_stmt->if_condition = if_condition;
    node->data.if_stmt->if_block = if_block;
    node->data.if_stmt->elif_conditions = elif_conditions;
    node->data.if_stmt->elif_blocks = elif_blocks;
    node->data.if_stmt->elif_blocks_count = elif_blocks_count;
    node->data.if_stmt->else_block = else_block;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_while(ast_expr_node_t *condition, ast_stmt_node_t *block, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_WHILE;
    node->data.while_stmt = malloc(sizeof(stmt_while_t));
    CHECK_MEM_ALLOC_ERROR(node->data.while_stmt);
    node->data.while_stmt->condition = condition;
    node->data.while_stmt->block = block;
    node->line = line;
    node->column = column;
    return node;
}

stmt_for_init_t *init_stmt_for_init_var_decl(const char *name, data_type_t type, ast_expr_node_t *expr, size_t line, size_t column) {
    stmt_for_init_t *init = malloc(sizeof(stmt_for_init_t));
    CHECK_MEM_ALLOC_ERROR(init);
    init->kind = FOR_INIT_VAR_DECL;
    init->line = line;
    init->column = column;
    init->data.var_decl = malloc(sizeof(stmt_var_decl_t));
    CHECK_MEM_ALLOC_ERROR(init->data.var_decl);
    init->data.var_decl->name = strdup(name);
    init->data.var_decl->type = type;
    init->data.var_decl->initializer = expr; // No initializer for var declaration in for loop
    return init;
}

stmt_for_init_t *init_stmt_for_init_assign(const char *name, ast_expr_node_t *value, size_t line, size_t column) {
    stmt_for_init_t *init = malloc(sizeof(stmt_for_init_t));
    CHECK_MEM_ALLOC_ERROR(init);
    init->kind = FOR_INIT_ASSIGN;
    init->line = line;
    init->column = column;
    init->data.assign = malloc(sizeof(stmt_assign_t));
    CHECK_MEM_ALLOC_ERROR(init->data.assign);
    init->data.assign->name = strdup(name);
    init->data.assign->value = value;
    return init;
}

stmt_for_init_t *init_stmt_for_init_expr(ast_expr_node_t *expression, size_t line, size_t column) {
    stmt_for_init_t *init = malloc(sizeof(stmt_for_init_t));
    CHECK_MEM_ALLOC_ERROR(init);
    init->kind = FOR_INIT_EXPR;
    init->line = line;
    init->column = column;
    init->data.expr = malloc(sizeof(stmt_expr_t));
    CHECK_MEM_ALLOC_ERROR(init->data.expr);
    init->data.expr->expression = expression;
    return init;
}

ast_stmt_node_t *init_stmt_for(stmt_for_init_t *init, ast_expr_node_t *condition, stmt_assign_t *increment, ast_stmt_node_t *block, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_FOR;
    node->data.for_stmt = malloc(sizeof(stmt_for_t));
    CHECK_MEM_ALLOC_ERROR(node->data.for_stmt);
    node->data.for_stmt->init = init;
    node->data.for_stmt->condition = condition;
    node->data.for_stmt->increment = increment;
    node->data.for_stmt->block = block;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_expr(ast_expr_node_t *expression, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_EXPR;
    node->data.expr_stmt = malloc(sizeof(stmt_expr_t));
    CHECK_MEM_ALLOC_ERROR(node->data.expr_stmt);
    node->data.expr_stmt->expression = expression;
    node->line = line;
    node->column = column;
    return node;
}

ast_stmt_node_t *init_stmt_block(ast_stmt_node_t **statements, size_t statement_count, size_t line, size_t column) {
    ast_stmt_node_t *node = malloc(sizeof(ast_stmt_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = STMT_BLOCK;
    node->data.block_stmt = malloc(sizeof(stmt_block_t));
    CHECK_MEM_ALLOC_ERROR(node->data.block_stmt);
    node->data.block_stmt->statements = statements;
    node->data.block_stmt->statement_count = statement_count;
    node->line = line;
    node->column = column;
    return node;
}


//-------------------- Declaration Node Initializers -------------------------------------------
param_t *init_decl_param(const char *name, data_type_t type, size_t line, size_t column) {
    param_t *param = malloc(sizeof(param_t));
    CHECK_MEM_ALLOC_ERROR(param);
    param->name = strdup(name);
    param->type = type;
    param->line = line;
    param->column = column;
    return param;
}

param_list_t *init_decl_param_list(param_t *params, size_t param_count, size_t line, size_t column) {
    (void)line;
    (void)column;
    param_list_t *param_list = malloc(sizeof(param_list_t));
    CHECK_MEM_ALLOC_ERROR(param_list);
    param_list->params = params;
    param_list->param_count = param_count;
    param_list->line = line;
    param_list->column = column;
    return param_list;
}

ast_decl_node_t *init_decl_function(const char *name, data_type_t return_type, param_list_t *params, 
                            ast_stmt_node_t **body, size_t body_count, size_t line, size_t column) {
    ast_decl_node_t *node = malloc(sizeof(ast_decl_node_t));
    CHECK_MEM_ALLOC_ERROR(node);
    node->type = DECL_FUNCTION;
    node->data.function_decl = malloc(sizeof(decl_function_t));
    CHECK_MEM_ALLOC_ERROR(node->data.function_decl);
    node->data.function_decl->name = strdup(name);
    node->data.function_decl->return_type = return_type;
    node->data.function_decl->param_list = params;
    node->data.function_decl->body = body;
    node->data.function_decl->body_count = body_count;
    node->line = line;
    node->column = column;
    return node;
}

void print_indent(int indent_level) {
    for (int i = 0; i < indent_level; ++i) {
        printf("  ");
    }
}

void print_ast(ast_t *ast) {
    printf("AST with %zu nodes:\n", ast->node_count);
    for (size_t i = 0; i < ast->node_count; ++i) {
        print_ast_node(ast->nodes[i], 1);
    }
}

void print_ast_node(ast_node_t *node, int indent_level) {
    switch (node->type) {
        case AST_NODE_CATEGORY_EXPR:
            print_expr(node->data.expr_node, indent_level);
            break;

        case AST_NODE_CATEGORY_STMT:
            print_stmt(node->data.stmt_node, indent_level);
            break;

        case AST_NODE_CATEGORY_DECL:
            print_decl(node->data.decl_node, indent_level);
            break;
    }
}

// ----------------------------- Expression Printer -----------------------------

void print_expr(ast_expr_node_t *expr, int indent) {
    print_indent(indent);
    switch (expr->type) {
        case EXPR_LITERAL_INT:
            printf("Literal Int: %d\n", expr->data.literal_int->value);
            break;

        case EXPR_LITERAL_FLOAT:
            printf("Literal Float: %f\n", expr->data.literal_float->value);
            break;

        case EXPR_LITERAL_STRING:
            printf("Literal String: \"%s\"\n", expr->data.literal_string->value);
            break;

        case EXPR_IDENTIFIER:
            printf("Identifier: %s\n", expr->data.identifier->name);
            break;

        case EXPR_ASSIGNMENT:
            printf("Assignment to %s:\n", expr->data.assignment->name);
            print_expr(expr->data.assignment->value, indent + 1);
            break;

        case EXPR_BINARY:
            printf("Binary Expression (%s):\n", token_type_to_string(expr->data.binary->operator));
            print_expr(expr->data.binary->left, indent + 1);
            print_expr(expr->data.binary->right, indent + 1);
            break;

        case EXPR_UNARY:
            printf("Unary Expression (%s):\n", token_type_to_string(expr->data.unary->operator));
            print_expr(expr->data.unary->operand, indent + 1);
            break;

        case EXPR_CALL:
            printf("Function Call: %s with %zu args\n", expr->data.call->name, expr->data.call->args->arg_count);
            for (size_t i = 0; i < expr->data.call->args->arg_count; ++i) {
                print_expr(expr->data.call->args->args[i], indent + 1);
            }
            break;

        case EXPR_ARG_LIST:
            printf("Argument List with %zu args\n", expr->data.arg_list->arg_count);
            for (size_t i = 0; i < expr->data.arg_list->arg_count; ++i) {
                print_expr(expr->data.arg_list->args[i], indent + 1);
            }
            break;
    }
}

// ----------------------------- Statement Printer -----------------------------

void print_stmt(ast_stmt_node_t *stmt, int indent) {
    print_indent(indent);
    switch (stmt->type) {
        case STMT_VAR_DECL:
            printf("Variable Declaration: %s (type %s)\n", stmt->data.var_decl->name, data_type_to_string(stmt->data.var_decl->type));
            if (stmt->data.var_decl->initializer) {
                print_expr(stmt->data.var_decl->initializer, indent + 1);
            }
            break;

        case STMT_ASSIGN:
            printf("Assignment Statement: %s\n", stmt->data.assign->name);
            print_expr(stmt->data.assign->value, indent + 1);
            break;

        case STMT_RETURN:
            printf("Return Statement:\n");
            if (stmt->data.return_stmt->value) {
                print_expr(stmt->data.return_stmt->value, indent + 1);
            }
            break;

        case STMT_PRINT:
            printf("Print Statement:\n");
            for (size_t i = 0; i < stmt->data.print_stmt->args->arg_count; ++i) {
                print_expr(stmt->data.print_stmt->args->args[i], indent + 1);
            }
            break;

        case STMT_BREAK:
            printf("Break Statement\n");
            break;

        case STMT_CONTINUE:
            printf("Continue Statement\n");
            break;

        case STMT_EXPR:
            printf("Expression Statement:\n");
            print_expr(stmt->data.expr_stmt->expression, indent + 1);
            break;

        case STMT_BLOCK: {
            printf("Block:\n");
            for (size_t i = 0; i < stmt->data.block_stmt->statement_count; ++i) {
                print_stmt(stmt->data.block_stmt->statements[i], indent + 1);
            }
            break;
        }

        case STMT_IF: {
            printf("If Statement:\n");
            print_indent(indent + 1);
            printf("If condition:\n");
            print_expr(stmt->data.if_stmt->if_condition, indent + 2);
            // print_indent(indent + 1);
            // printf("If branch:\n");
            // for (size_t i = 0; i < stmt->data.if_stmt->if_branch_size; ++i) {
            //     print_stmt(stmt->data.if_stmt->if_branch[i], indent + 2);
            // }
            print_stmt(stmt->data.if_stmt->if_block, indent + 1);
            for (size_t i = 0; i < stmt->data.if_stmt->elif_blocks_count; ++i) {
                print_indent(indent + 1);
                printf("Elif condition %zu:\n", i);
                // print_expr(stmt->data.if_stmt->elif_conditions[i], indent + 2);
                // for (size_t j = 0; j < stmt->data.if_stmt->elif_branch_size[i]; ++j) {
                //     print_stmt(stmt->data.if_stmt->elif_branches[i][j], indent + 2);
                // }
                print_stmt(stmt->data.if_stmt->elif_blocks[i], indent + 1);
            }
            // if (stmt->data.if_stmt->else_branch_size > 0) {
            //     print_indent(indent + 1);
            //     printf("Else branch:\n");
            //     for (size_t i = 0; i < stmt->data.if_stmt->else_branch_size; ++i) {
            //         print_stmt(stmt->data.if_stmt->else_branch[i], indent + 2);
            //     }
            // }
            if (stmt->data.if_stmt->else_block != NULL) {
                print_stmt(stmt->data.if_stmt->else_block, indent + 1);
            }
            break;
        }

        case STMT_WHILE: {
            printf("While Statement:\n");
            print_expr(stmt->data.while_stmt->condition, indent + 1);
            print_stmt(stmt->data.while_stmt->block, indent + 1);
            break;
        }

        case STMT_FOR: {
            printf("For Statement:\n");
            stmt_for_t *for_stmt = stmt->data.for_stmt;
            if (for_stmt->init) {
                print_indent(indent + 1);
                printf("Initializer:\n");
                switch (for_stmt->init->kind) {
                    case FOR_INIT_VAR_DECL:
                        if (for_stmt->init->data.var_decl) {
                            stmt_var_decl_t *var_decl = for_stmt->init->data.var_decl;
                            print_indent(indent + 2);
                            printf("Variable Declaration: %s (type: %s)\n", var_decl->name, data_type_to_string(var_decl->type));
                            if (var_decl->initializer) {
                                print_indent(indent + 3);
                                printf("Initializer:\n");
                                print_expr(var_decl->initializer, indent + 4);
                            }
                        }
                        break;
                    case FOR_INIT_ASSIGN:
                        if (for_stmt->init->data.assign) {
                            stmt_assign_t *assign = for_stmt->init->data.assign;
                            print_indent(indent + 2);
                            printf("Assignment: %s =\n", assign->name);
                            print_expr(assign->value, indent + 3);
                        }
                        break;
                    case FOR_INIT_EXPR:
                        if (for_stmt->init->data.expr) {
                            print_indent(indent + 2);
                            printf("Expression:\n");
                            print_expr(for_stmt->init->data.expr->expression, indent + 3);
                        }
                        break;
                    case FOR_INIT_NONE:
                        print_indent(indent + 2);
                        printf("No initializer\n");
                        break;
                }
            }

            if (for_stmt->condition) {
                print_indent(indent + 1);
                printf("Condition:\n");
                print_expr(for_stmt->condition, indent + 2);
            }

            if (for_stmt->increment) {
                print_indent(indent + 1);
                printf("Increment:\n");
                print_indent(indent + 2);
                printf("%s =\n", for_stmt->increment->name);
                print_expr(for_stmt->increment->value, indent + 3);
            }

            print_stmt(stmt->data.for_stmt->block, indent + 1);

            break;
        }

    }
}

// ----------------------------- Declaration Printer -----------------------------

void print_decl(ast_decl_node_t *decl, int indent) {
    print_indent(indent);
    switch (decl->type) {
        case DECL_FUNCTION:
            printf("Function Declaration: %s (return type %s)\n", decl->data.function_decl->name, data_type_to_string(decl->data.function_decl->return_type));
            print_indent(indent + 1);
            printf("Parameters:\n");
            for (size_t i = 0; i < decl->data.function_decl->param_list->param_count; ++i) {
                print_indent(indent + 2);
                printf("Param: %s (type %s)\n", 
                    decl->data.function_decl->param_list->params[i].name, 
                    data_type_to_string(decl->data.function_decl->param_list->params[i].type));
            }
            print_indent(indent + 1);
            printf("Body:\n");
            for (size_t i = 0; i < decl->data.function_decl->body_count; ++i) {
                print_stmt(decl->data.function_decl->body[i], indent + 2);
            }
            break;

    }
}
