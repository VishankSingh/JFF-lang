/**
 * File Name: parser.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include "include/parser.h"
#include "include/ast.h"
#include "include/utils.h"

parser_t *init_parser(lexer_t *lexer) {
    parser_t *parser = malloc(sizeof(parser_t));
    CHECK_MEM_ALLOC_ERROR(parser);
    parser->tokens = lexer->tokens;
    parser->token_count = lexer->token_count;
    parser->current_index = 0;
    parser->lexer = lexer;
    parser->current = lexer->tokens[0];
    parser->previous = lexer->tokens[0];
    parser->ast = init_ast();
    return parser;
}

void free_parser(parser_t *parser) {
    free_ast(parser->ast);
    free(parser);
}

void parser_advance(parser_t *parser) {
    parser->previous = parser->current;
    if (parser->current_index < parser->token_count - 1) {
        parser->current = parser->tokens[++parser->current_index];
    } else {
        parser->current = NULL;
    }
}

/**
 * @brief Check if the current token matches the expected type.
 * 
 * @param parser The parser instance.
 * @param type The expected token type.
 * @return true if the current token matches the expected type, false otherwise.
 */
bool parser_match(parser_t *parser, token_type_t type) {
    if (parser->current && parser->current->type == type) {
        return true;
    }
    return false;
}

token_t *parser_peek_token(parser_t *parser, size_t dist) {
    if (dist < parser->token_count) {
        return parser->tokens[parser->current_index + dist];
    }
    return NULL;
}

void parser_expect_advance(parser_t *parser, token_type_t type) {
    if (!parser_match(parser, type)) {
        fprintf(stderr, "[%ld:%ld] Expected token type <%s> but got <%s>\n",parser->current->line, parser->current->column,  token_type_to_string(type), token_type_to_string(parser->current->type));
        exit(EXIT_FAILURE);
    }
    parser_advance(parser);
}

void parser_expect(parser_t *parser, token_type_t type) {
    if (!parser_match(parser, type)) {
        fprintf(stderr, "Expected token type %s but got %s\n", token_type_to_string(type), token_type_to_string(parser->current->type));
        exit(EXIT_FAILURE);
    }
}

void parser_parse_program(parser_t *parser) {
    while (parser->current && parser->current->type != TOKEN_EOF) {
        ast_node_t *node = parser_parse_declaration(parser);
        if (node) {
            if (parser->ast->node_count >= parser->ast->nodes_capacity) {
                parser->ast->nodes_capacity *= 2;
                ast_node_t **new_nodes = realloc(parser->ast->nodes, parser->ast->nodes_capacity * sizeof(ast_node_t *));
                CHECK_MEM_ALLOC_ERROR(new_nodes);
                parser->ast->nodes = new_nodes;
            }
            parser->ast->nodes[parser->ast->node_count++] = node;
        }
    }
}

ast_node_t *parser_parse_declaration(parser_t *parser) {
    if (parser_match(parser, TOKEN_FUNC)) {
        ast_node_t *decl_node = init_ast_node(AST_NODE_CATEGORY_DECL, parser->current->line, parser->current->column);
        decl_node->data.decl_node = parser_parse_function_decl(parser);
        return decl_node;
    } else if (parser_match(parser, TOKEN_IDENTIFIER)) {
        ast_node_t *decl_node = init_ast_node(AST_NODE_CATEGORY_STMT, parser->current->line, parser->current->column);
        decl_node->data.stmt_node = parser_parse_var_decl(parser);;
        parser_expect_advance(parser, TOKEN_SEMICOLON);
        return decl_node;
    } else if (parser_match(parser, TOKEN_EOF)) {
        return NULL;
    } else {
        fprintf(stderr, "Expected function or variable declaration but got %s\n", parser->current->value);
        exit(EXIT_FAILURE);
    }

}

ast_decl_node_t *parser_parse_function_decl(parser_t *parser) {
    size_t line = parser->current->line;
    size_t column = parser->current->column;
    parser_expect_advance(parser, TOKEN_FUNC);
    char *name = parser->current->value;
    parser_expect_advance(parser, TOKEN_IDENTIFIER);
    parser_expect_advance(parser, TOKEN_LPAREN);

    param_list_t *param_list = NULL;
    if (parser->current && parser->current->type != TOKEN_RPAREN) {
        param_list = parser_parse_param_list(parser);
    } else {
        param_list = malloc(sizeof(param_list_t));
        CHECK_MEM_ALLOC_ERROR(param_list);
        param_list->params = NULL;
        param_list->param_count = 0;
    }

    parser_expect_advance(parser, TOKEN_RPAREN);
    parser_expect_advance(parser, TOKEN_COLON);
    data_type_t return_type = parser_parse_type(parser);
    parser_expect_advance(parser, TOKEN_LBRACE);
    //--------------------------- function body --------------------------------
    size_t body_capacity = 1;
    size_t body_count = 0;
    ast_stmt_node_t **body = malloc(sizeof(ast_stmt_node_t *) * body_capacity);
    CHECK_MEM_ALLOC_ERROR(body);



    while (parser->current && parser->current->type != TOKEN_RBRACE) {
        if (body_count >= body_capacity) {
            body_capacity *= 2;
            body = realloc(body, sizeof(ast_stmt_node_t *) * body_capacity);
            CHECK_MEM_ALLOC_ERROR(body);
        }
        body[body_count++] = parser_parse_statement(parser);
    }

    //--------------------------------------------------------------------------

    parser_expect_advance(parser, TOKEN_RBRACE);
    return init_decl_function(name, return_type, param_list, body, body_count, line, column);
}

param_list_t *parser_parse_param_list(parser_t *parser) {
    size_t param_capacity = 1;
    size_t param_count = 0;
    param_t *params = malloc(sizeof(param_t) * param_capacity);
    CHECK_MEM_ALLOC_ERROR(params);
    param_t *param = parser_parse_param(parser);
    params[param_count++] = *param;
    free(param);
    while (parser_match(parser, TOKEN_COMMA)) {
        parser_advance(parser);
        param = parser_parse_param(parser);
        if (param_count >= param_capacity) {
            param_capacity *= 2;
            params = realloc(params, sizeof(param_t) * param_capacity);
            CHECK_MEM_ALLOC_ERROR(params);
        }
        params[param_count++] = *param;
        free(param);
    }
    param_list_t *param_list = malloc(sizeof(param_list_t));
    CHECK_MEM_ALLOC_ERROR(param_list);
    param_list->params = params;
    param_list->param_count = param_count;
    param_list->line = parser->current->line;
    param_list->column = parser->current->column;
    return param_list;
}

param_t *parser_parse_param(parser_t *parser) {
    char *name = parser->current->value;
    parser_expect_advance(parser, TOKEN_IDENTIFIER);
    parser_expect_advance(parser, TOKEN_COLON);
    data_type_t type = parser_parse_type(parser);
    param_t *param = malloc(sizeof(param_t));
    CHECK_MEM_ALLOC_ERROR(param);
    param->name = strdup(name);
    param->type = type;
    param->line = parser->current->line;
    param->column = parser->current->column;
    return param;
}

data_type_t parser_parse_type(parser_t *parser) {
    switch (parser->current->type) {
        case TOKEN_TYPE_INT:
            parser_advance(parser);
            return DATA_TYPE_INT;
        case TOKEN_TYPE_FLOAT:
            parser_advance(parser);
            return DATA_TYPE_FLOAT;
        case TOKEN_TYPE_STRING:
            parser_advance(parser);
            return DATA_TYPE_STRING;
        case TOKEN_TYPE_BOOL:
            parser_advance(parser);
            return DATA_TYPE_BOOL;
        case TOKEN_TYPE_VOID:
            parser_advance(parser);
            return DATA_TYPE_VOID;
        default:
            fprintf(stderr, "Expected type, got %d\n", parser->current->type);
            exit(1);
    }
}

ast_stmt_node_t *parser_parse_var_decl(parser_t *parser) {
    size_t line = parser->current->line;
    size_t column = parser->current->column;
    parser_expect_advance(parser, TOKEN_IDENTIFIER);
    char *name = parser->previous->value;
    parser_expect_advance(parser, TOKEN_COLON);
    data_type_t type = parser_parse_type(parser);
    parser_expect_advance(parser, TOKEN_EQ);
    ast_expr_node_t *expr_initializer = parser_parse_expression(parser);
    ast_stmt_node_t *node = init_stmt_var_decl(name, type, expr_initializer, line, column);
    return node;
}

ast_stmt_node_t *parser_parse_statement(parser_t *parser) {
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        token_t *next = parser_peek_token(parser, 1);
        if (next->type == TOKEN_COLON) {
            ast_stmt_node_t *node = parser_parse_var_decl(parser);
            parser_expect_advance(parser, TOKEN_SEMICOLON);
            return node;
        } else if (next->type == TOKEN_EQ) {
            ast_stmt_node_t *node = parser_parse_assignment(parser);
            parser_expect_advance(parser, TOKEN_SEMICOLON);
            return node;
        } else {
            ast_expr_node_t *expr = parser_parse_expression(parser);
            ast_stmt_node_t *stmt = init_stmt_expr(expr, parser->current->line, parser->current->column);
            parser_expect_advance(parser, TOKEN_SEMICOLON);
            return stmt;
        }
    } else if (parser_match(parser, TOKEN_PRINT)) {
        ast_stmt_node_t *stmt = parser_parse_print_statement(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
        return stmt;
    } else if (parser_match(parser, TOKEN_LBRACE)) {
        return parser_parse_block_statement(parser);
    } else if (parser_match(parser, TOKEN_RETURN)) {
        ast_stmt_node_t *stmt = parser_parse_return_statement(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
        return stmt;
    } else if (parser_match(parser, TOKEN_BREAK)) {
        ast_stmt_node_t *stmt = init_stmt_break(parser->current->line, parser->current->column);
        parser_advance(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
        return stmt;
    } else if (parser_match(parser, TOKEN_CONTINUE)) {
        ast_stmt_node_t *stmt = init_stmt_continue(parser->current->line, parser->current->column);
        parser_advance(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
        return stmt;
    } else if (parser_match(parser, TOKEN_IF)) {
        // parser_parse_if_statement(parser);
        ast_stmt_node_t *stmt = parser_parse_if_statement(parser);
        return stmt;
    } else if (parser_match(parser, TOKEN_FOR)) {
        ast_stmt_node_t *stmt = parser_parse_for_statement(parser);
        return stmt;
    } else if (parser_match(parser, TOKEN_WHILE)) {
        ast_stmt_node_t *stmt = parser_parse_while_statement(parser);
        return stmt;
    } else if (parser_match(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
    } else {
        ast_expr_node_t *expr = parser_parse_expression(parser);
        ast_stmt_node_t *stmt = init_stmt_expr(expr, parser->current->line, parser->current->column);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
        return stmt;
    }
    ast_expr_node_t *lit_int = init_expr_literal_int(0, parser->current->line, parser->current->column);
    ast_stmt_node_t *dummy_node = init_stmt_assign("test", lit_int, parser->current->line, parser->current->column);
    return dummy_node;
}

ast_stmt_node_t *parser_parse_assignment(parser_t *parser) {
    char *name = parser->current->value;
    parser_expect_advance(parser, TOKEN_IDENTIFIER);
    parser_expect_advance(parser, TOKEN_EQ);
    ast_expr_node_t *value = parser_parse_expression(parser);
    return init_stmt_assign(name, value, parser->current->line, parser->current->column);
}

ast_stmt_node_t *parser_parse_if_statement(parser_t *parser) {
    size_t line = parser->current->line;
    size_t column = parser->current->column;

    //======================== if block ===========================================
    parser_expect_advance(parser, TOKEN_IF);
    parser_expect_advance(parser, TOKEN_LPAREN);
    ast_expr_node_t *if_condition = parser_parse_expression(parser);
    parser_expect_advance(parser, TOKEN_RPAREN);
    ast_stmt_node_t *if_block = parser_parse_block_statement(parser);
    //=============================================================================


    //======================== else if block ======================================
    size_t elif_capacity = 1;
    size_t elif_count = 0;
    ast_expr_node_t **elif_conditions = malloc(sizeof(ast_expr_node_t *) * elif_capacity);
    CHECK_MEM_ALLOC_ERROR(elif_conditions);
    ast_stmt_node_t **elif_blocks = malloc(sizeof(ast_stmt_node_t *) * elif_capacity);
    CHECK_MEM_ALLOC_ERROR(elif_blocks);
    
    while (parser_match(parser, TOKEN_ELIF)) {
        parser_expect_advance(parser, TOKEN_ELIF);
        parser_expect_advance(parser, TOKEN_LPAREN);
        ast_expr_node_t *elif_condition = parser_parse_expression(parser);
        parser_expect_advance(parser, TOKEN_RPAREN);
        ast_stmt_node_t *elif_block = parser_parse_block_statement(parser);

        if (elif_count >= elif_capacity) {
            elif_capacity *= 2;
            elif_conditions = realloc(elif_conditions, sizeof(ast_expr_node_t *) * elif_capacity);
            CHECK_MEM_ALLOC_ERROR(elif_conditions);
            elif_blocks = realloc(elif_blocks, sizeof(ast_stmt_node_t *) * elif_capacity);
            CHECK_MEM_ALLOC_ERROR(elif_blocks);
        }
        elif_conditions[elif_count] = elif_condition;
        elif_blocks[elif_count] = elif_block;
        elif_count++;
    }
    //=============================================================================
    
    //======================== else block =========================================
    ast_stmt_node_t *else_block = NULL;
    if (parser_match(parser, TOKEN_ELSE)) {
        parser_expect_advance(parser, TOKEN_ELSE);
        else_block = parser_parse_block_statement(parser);
    }
    //=============================================================================

    return init_stmt_if(if_condition, if_block, elif_conditions, elif_count, elif_blocks, else_block, line, column);
}

ast_stmt_node_t *parser_parse_for_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_FOR);
    size_t line = parser->previous->line;
    size_t column = parser->previous->column;

    parser_expect_advance(parser, TOKEN_LPAREN);

    //==================== for init ===============================================
    stmt_for_init_t *init = NULL;
    if (parser->current && parser->current->type != TOKEN_SEMICOLON) {
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            token_t *identifier = parser->current;
            token_t *next = parser_peek_token(parser, 1);
            if (next->type == TOKEN_COLON) {
                parser_advance(parser); // skip identifier
                parser_expect_advance(parser, TOKEN_COLON);
                data_type_t type = parser_parse_type(parser);
                parser_expect_advance(parser, TOKEN_EQ);
                ast_expr_node_t *value = parser_parse_expression(parser);
                init = init_stmt_for_init_var_decl(identifier->value, type, value, line, column);
            } else if (next->type == TOKEN_EQ) {
                parser_parse_assignment(parser);
            } else {
                parser_parse_expression(parser); 
            }
        } else {
            parser_parse_expression(parser);
        }
    }
    //=============================================================================

    parser_expect_advance(parser, TOKEN_SEMICOLON);

    //==================== for condition ==========================================
    ast_expr_node_t *condition = NULL;
    if (parser->current && parser->current->type != TOKEN_SEMICOLON) {
        condition = parser_parse_expression(parser);
    }
    //=============================================================================
    
    //==================== for increment ==========================================
    parser_expect_advance(parser, TOKEN_SEMICOLON);
    stmt_assign_t *increment = NULL;
    if (parser->current && parser->current->type != TOKEN_RPAREN) {
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            token_t *identifier = parser->current;
            parser_advance(parser); // skip identifier
            parser_expect_advance(parser, TOKEN_EQ);
            ast_expr_node_t *value = parser_parse_expression(parser);
            increment = malloc(sizeof(stmt_assign_t));
            CHECK_MEM_ALLOC_ERROR(increment);
            increment->name = strdup(identifier->value);
            increment->value = value;
        } else {
            parser_parse_expression(parser);
        }
    }
    //=============================================================================
    parser_expect_advance(parser, TOKEN_RPAREN);

    //==================== for block ==============================================
    ast_stmt_node_t *block = parser_parse_block_statement(parser);
    //=============================================================================
    ast_stmt_node_t *stmt = init_stmt_for(init, condition, increment, block, line, column);
    return stmt;
}

ast_stmt_node_t *parser_parse_while_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_WHILE);
    parser_expect_advance(parser, TOKEN_LPAREN);

    //==================== while condition ========================================
    ast_expr_node_t *condition = parser_parse_expression(parser);
    //=============================================================================
    
    parser_expect_advance(parser, TOKEN_RPAREN);
    
    //==================== while block ============================================
    ast_stmt_node_t *block = parser_parse_block_statement(parser);
    //=============================================================================

    ast_stmt_node_t *stmt = init_stmt_while(condition, block, parser->previous->line, parser->previous->column);
    return stmt;
}

ast_stmt_node_t *parser_parse_print_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_PRINT);
    parser_expect_advance(parser, TOKEN_LPAREN);
    expr_arg_list_t *args = parser_parse_arg_list(parser);
    parser_expect_advance(parser, TOKEN_RPAREN);
    ast_stmt_node_t *stmt = init_stmt_print(args, parser->current->line, parser->current->column);
    return stmt;
}

ast_stmt_node_t *parser_parse_return_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_RETURN);
    ast_expr_node_t *value = NULL;
    if (parser->current && parser->current->type != TOKEN_SEMICOLON) {
        value = parser_parse_expression(parser);
    }

    ast_stmt_node_t *stmt = init_stmt_return(value, parser->current->line, parser->current->column);
    return stmt;
}

ast_stmt_node_t *parser_parse_block_statement(parser_t *parser) {
    size_t line = parser->current->line;
    size_t column = parser->current->column;
    parser_expect_advance(parser, TOKEN_LBRACE);
    size_t body_capacity = 1;
    size_t body_count = 0;
    ast_stmt_node_t **body = malloc(sizeof(ast_stmt_node_t *) * body_capacity);
    CHECK_MEM_ALLOC_ERROR(body);
    while (parser->current && parser->current->type != TOKEN_RBRACE) {
        if (body_count >= body_capacity) {
            body_capacity *= 2;
            body = realloc(body, sizeof(ast_stmt_node_t *) * body_capacity);
            CHECK_MEM_ALLOC_ERROR(body);
        }
        body[body_count++] = parser_parse_statement(parser);
    }
    parser_expect_advance(parser, TOKEN_RBRACE);
    ast_stmt_node_t *stmt = init_stmt_block(body, body_count, line, column);
    return stmt;
}

ast_expr_node_t *parser_parse_expression(parser_t *parser) {
    return parser_parse_logical_or(parser);
}

ast_expr_node_t *parser_parse_logical_or(parser_t *parser) {
    ast_expr_node_t *left = parser_parse_logical_and(parser);
    
    while (parser_match(parser, TOKEN_OR)) {
        parser_advance(parser);
        ast_expr_node_t *right = parser_parse_logical_and(parser);
        left = init_expr_binary(TOKEN_OR, left, right, parser->current->line, parser->current->column);
    }
    return left;
}

ast_expr_node_t *parser_parse_logical_and(parser_t *parser) {
    ast_expr_node_t *left = parser_parse_equality(parser);
    while (parser_match(parser, TOKEN_AND)) {
        parser_advance(parser);
        ast_expr_node_t *right = parser_parse_equality(parser);
        left = init_expr_binary(TOKEN_AND, left, right, parser->current->line, parser->current->column);
    }
    return left;
}

ast_expr_node_t *parser_parse_equality(parser_t *parser) {
    ast_expr_node_t *left = parser_parse_comparasion(parser);
    while (parser_match(parser, TOKEN_EQEQ) || parser_match(parser, TOKEN_NEQ)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *right = parser_parse_comparasion(parser);
        left = init_expr_binary(operator, left, right, parser->current->line, parser->current->column);
    }
    return left;
}

ast_expr_node_t *parser_parse_comparasion(parser_t *parser) {
    ast_expr_node_t *left = parser_parse_term(parser);
    while (parser_match(parser, TOKEN_GT) 
        || parser_match(parser, TOKEN_GEQ)
        || parser_match(parser, TOKEN_LT)
        || parser_match(parser, TOKEN_LEQ)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *right = parser_parse_term(parser);
        left = init_expr_binary(operator, left, right, parser->current->line, parser->current->column);
    }
    return left;
}

ast_expr_node_t *parser_parse_term(parser_t *parser) {
    ast_expr_node_t *left =  parser_parse_factor(parser);
    while (parser_match(parser, TOKEN_PLUS) || parser_match(parser, TOKEN_MINUS)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *right = parser_parse_factor(parser);
        left = init_expr_binary(operator, left, right, parser->current->line, parser->current->column);
    }
    return left;
}

ast_expr_node_t *parser_parse_factor(parser_t *parser) {
    ast_expr_node_t *left =  parser_parse_unary(parser);
    while (parser_match(parser, TOKEN_ASTERISK) || parser_match(parser, TOKEN_SLASH) || parser_match(parser, TOKEN_PERCENT)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *right = parser_parse_unary(parser);
        left = init_expr_binary(operator, left, right, parser->current->line, parser->current->column);
    }
    return left;
}

ast_expr_node_t *parser_parse_unary(parser_t *parser) {
    size_t line = parser->current->line;
    size_t column = parser->current->column;
    if (parser_match(parser, TOKEN_NOT)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *operand = parser_parse_unary(parser);
        return init_expr_unary(operator, operand, line, column);
    } else if (parser_match(parser, TOKEN_MINUS)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *operand = parser_parse_unary(parser);
        return init_expr_unary(operator, operand, line, column);
    } else if (parser_match(parser, TOKEN_PLUS)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *operand = parser_parse_unary(parser);
        return init_expr_unary(operator, operand, line, column);
    } else if (parser_match(parser, TOKEN_PLUSPLUS)) {
        token_type_t operator = parser->current->type;
        parser_advance(parser);
        ast_expr_node_t *operand = parser_parse_unary(parser);
        return init_expr_unary(operator, operand, line, column);
    }
    return parser_parse_primary(parser);
}

ast_expr_node_t *parser_parse_primary(parser_t *parser) {
    if (parser->current->type == TOKEN_LITERAL_INT) {
        ast_expr_node_t *node = init_expr_literal_int(atoi(parser->current->value), parser->current->line, parser->current->column);
        parser_advance(parser);
        return node;
    } else if (parser->current->type == TOKEN_LITERAL_FLOAT) {
        ast_expr_node_t *node = init_expr_literal_float(atof(parser->current->value), parser->current->line, parser->current->column);
        parser_advance(parser);
        return node;
    } else if (parser->current->type == TOKEN_LITERAL_STR) {
        ast_expr_node_t *node = init_expr_literal_string(parser->current->value, parser->current->line, parser->current->column);
        parser_advance(parser);
        return node;
    } else if (parser->current->type == TOKEN_TRUE || parser->current->type == TOKEN_FALSE) {
        // TODO: Handle boolean literals
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_NULL) {
        // TODO: Handle null literal
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_IDENTIFIER && parser_peek_token(parser, 1)->type == TOKEN_LPAREN) {
        char *name = parser->current->value;
        parser_advance(parser);
        parser_expect_advance(parser, TOKEN_LPAREN);
        expr_arg_list_t *arg_list = NULL;
        if (parser->current && parser->current->type != TOKEN_RPAREN) {
            arg_list = parser_parse_arg_list(parser);
        } else {
            arg_list = malloc(sizeof(expr_arg_list_t));
            CHECK_MEM_ALLOC_ERROR(arg_list);
            arg_list->args = NULL;
            arg_list->arg_count = 0;
        }
        ast_expr_node_t *node = init_expr_call(name, arg_list, parser->current->line, parser->current->column);
        parser_expect_advance(parser, TOKEN_RPAREN);
        return node;
    } else if (parser->current->type == TOKEN_IDENTIFIER) {
        ast_expr_node_t *node = init_expr_identifier(parser->current->value, parser->current->line, parser->current->column);
        parser_advance(parser);
        return node;
    } else if (parser->current->type == TOKEN_LPAREN) {
        parser_advance(parser);
        ast_expr_node_t *expr = parser_parse_expression(parser);
        parser_expect_advance(parser, TOKEN_RPAREN);
        return expr;
    } else {
        fprintf(stderr, "[%ld:%ld] Expected primary expression but got %s\n", parser->current->line, parser->current->column, parser->current->value);
        exit(EXIT_FAILURE);
    }
    return NULL;
}

expr_arg_list_t *parser_parse_arg_list(parser_t *parser) {
    size_t arg_capacity = 8;
    size_t arg_count = 0;

    ast_expr_node_t **args = malloc(sizeof(ast_expr_node_t *) * arg_capacity);
    CHECK_MEM_ALLOC_ERROR(args);

    while (parser->current && parser->current->type != TOKEN_RPAREN) {
        if (arg_count >= arg_capacity) {
            arg_capacity *= 2;
            ast_expr_node_t **args_new = realloc(args, sizeof(ast_expr_node_t *) * arg_capacity);
            CHECK_MEM_ALLOC_ERROR(args_new);
            args = args_new;
        }

        args[arg_count++] = parser_parse_expression(parser);

        if (parser_match(parser, TOKEN_COMMA)) {
            parser_advance(parser);
        } else {
            break;
        }
    }

    expr_arg_list_t *arg_list = malloc(sizeof(expr_arg_list_t));
    CHECK_MEM_ALLOC_ERROR(arg_list);
    arg_list->args = args;
    arg_list->arg_count = arg_count;

    return arg_list;
}
