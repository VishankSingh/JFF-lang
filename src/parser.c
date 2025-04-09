/**
 * File Name: parser.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include "include/parser.h"

parser_t *init_parser(lexer_t *lexer) {
    parser_t *parser = malloc(sizeof(parser_t));
    if (!parser) {
        fprintf(stderr, "Failed to allocate memory for parser\n");
        return NULL;
    }
    parser->tokens = lexer->tokens;
    parser->token_count = lexer->token_count;
    parser->current_index = 0;
    parser->lexer = lexer;
    parser->current = lexer->tokens[0];
    parser->previous = lexer->tokens[0];
    return parser;
}

void free_parser(parser_t *parser) {
    if (parser) {
        free(parser);
    }
}

void parser_advance(parser_t *parser) {
    parser->previous = parser->current;
    if (parser->current_index < parser->token_count - 1) {
        parser->current = parser->tokens[++parser->current_index];
    } else {
        parser->current = NULL;
    }
    // printf("%s\n", parser->previous->value);
}

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
        parser_parse_declaration(parser);
    }
}

void parser_parse_declaration(parser_t *parser) {
    if (parser_match(parser, TOKEN_FUNC)) {
        parser_parse_function_decl(parser);
    } else if (parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_parse_var_decl(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
    } else if (parser_match(parser, TOKEN_EOF)) {
        return;
    } else {
        fprintf(stderr, "Expected function or variable declaration but got %s\n", parser->current->value);
        exit(EXIT_FAILURE);
    }
}

void parser_parse_function_decl(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_FUNC);
    parser_expect_advance(parser, TOKEN_IDENTIFIER);
    parser_expect_advance(parser, TOKEN_LPAREN);
    if (parser->current && parser->current->type != TOKEN_RPAREN) {
        parser_parse_param_list(parser);
    }
    parser_expect_advance(parser, TOKEN_RPAREN);
    parser_expect_advance(parser, TOKEN_COLON);
    parser_parse_type(parser);
    parser_expect_advance(parser, TOKEN_LBRACE);
    while (parser->current && parser->current->type != TOKEN_RBRACE) {
        parser_parse_statement(parser);
    }
    parser_expect_advance(parser, TOKEN_RBRACE);
}

void parser_parse_param_list(parser_t *parser) {
    parser_parse_param(parser);
    while (parser_match(parser, TOKEN_COMMA)) {
        parser_parse_param(parser);
    }
}

void parser_parse_param(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_IDENTIFIER);
    parser_expect_advance(parser, TOKEN_COLON);
    parser_parse_type(parser);
}

void parser_parse_type(parser_t *parser) {
    switch (parser->current->type) {
        case TOKEN_TYPE_INT:
        case TOKEN_TYPE_FLOAT:
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_BOOL:
        case TOKEN_TYPE_VOID:
            parser_advance(parser);
            break;
        default:
            fprintf(stderr, "Expected type, got %d\n", parser->current->type);
            exit(1);
    }
}

void parser_parse_var_decl(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_IDENTIFIER);
    parser_expect_advance(parser, TOKEN_COLON);
    parser_parse_type(parser);
    parser_expect_advance(parser, TOKEN_EQ);
    parser_parse_expression(parser);
}

void parser_parse_statement(parser_t *parser) {
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        token_t *next = parser_peek_token(parser, 1);
        if (next->type == TOKEN_COLON) {
            parser_parse_var_decl(parser);
            parser_expect_advance(parser, TOKEN_SEMICOLON);
        } else if (next->type == TOKEN_EQ) {
            parser_parse_assignment(parser);
            parser_expect_advance(parser, TOKEN_SEMICOLON);
        } else {
            parser_parse_expression(parser);
            parser_expect_advance(parser, TOKEN_SEMICOLON);
        }
    } else if (parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_parse_var_decl(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
    } else if (parser_match(parser, TOKEN_PRINT)) {
        parser_parse_print_statement(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
    } else if (parser_match(parser, TOKEN_RETURN)) {
        parser_parse_return_statement(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
    } else if (parser_match(parser, TOKEN_BREAK)) {
        parser_advance(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
    } else if (parser_match(parser, TOKEN_CONTINUE)) {
        parser_advance(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
    } else if (parser_match(parser, TOKEN_IF)) {
        parser_parse_if_statement(parser);
    } else if (parser_match(parser, TOKEN_FOR)) {
        parser_parse_for_statement(parser);
    } else if (parser_match(parser, TOKEN_WHILE)) {
        parser_parse_while_statement(parser);
    } else if (parser_match(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
    } else {
        parser_parse_expression(parser);
        parser_expect_advance(parser, TOKEN_SEMICOLON);
    }
}

void parser_parse_if_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_IF);
    parser_expect_advance(parser, TOKEN_LPAREN);
    parser_parse_expression(parser);
    parser_expect_advance(parser, TOKEN_RPAREN);
    parser_expect_advance(parser, TOKEN_LBRACE);
    while (parser->current && parser->current->type != TOKEN_RBRACE) {
        parser_parse_statement(parser);
    }
    parser_expect_advance(parser, TOKEN_RBRACE);
    if (parser_match(parser, TOKEN_ELIF)) {
        parser_expect_advance(parser, TOKEN_ELIF);
        parser_expect_advance(parser, TOKEN_LPAREN);
        parser_parse_expression(parser);
        parser_expect_advance(parser, TOKEN_RPAREN);
        parser_expect_advance(parser, TOKEN_LBRACE);
        while (parser->current && parser->current->type != TOKEN_RBRACE) {
            parser_parse_statement(parser);
        }
        parser_expect_advance(parser, TOKEN_RBRACE);
    } else if (parser_match(parser, TOKEN_ELSE)) {
        parser_expect_advance(parser, TOKEN_ELSE);
        parser_expect_advance(parser, TOKEN_LBRACE);
        while (parser->current && parser->current->type != TOKEN_RBRACE) {
            parser_parse_statement(parser);
        }
        parser_expect_advance(parser, TOKEN_RBRACE);
    }
}

void parser_parse_for_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_FOR);
    parser_expect_advance(parser, TOKEN_LPAREN);
    if (parser->current && parser->current->type != TOKEN_SEMICOLON) {
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            token_t *next = parser_peek_token(parser, 1);
            if (next->type == TOKEN_COLON) {
                parser_parse_var_decl(parser);
            } else if (next->type == TOKEN_EQ) {
                parser_parse_assignment(parser);
            } else {
                parser_parse_expression(parser); // fallback: e.g. `foo();`
            }
        } else {
            parser_parse_expression(parser);
        }
    }
    parser_expect_advance(parser, TOKEN_SEMICOLON);
    if (parser->current && parser->current->type != TOKEN_SEMICOLON) {
        parser_parse_expression(parser);
    }
    parser_expect_advance(parser, TOKEN_SEMICOLON);
    if (parser->current && parser->current->type != TOKEN_RPAREN) {
        parser_parse_assignment(parser);
    }
    parser_expect_advance(parser, TOKEN_RPAREN);
    parser_expect_advance(parser, TOKEN_LBRACE);
    while (parser->current && parser->current->type != TOKEN_RBRACE) {
        parser_parse_statement(parser);
    }
    parser_expect_advance(parser, TOKEN_RBRACE);
}

void parser_parse_while_statement(parser_t *parser) {
    
}



void parser_parse_print_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_PRINT);
    parser_expect_advance(parser, TOKEN_LPAREN);
    parser_parse_arg_list(parser);
    parser_expect_advance(parser, TOKEN_RPAREN);
}

void parser_parse_return_statement(parser_t *parser) {
    parser_expect_advance(parser, TOKEN_RETURN);
    if (parser->current && parser->current->type != TOKEN_SEMICOLON) {
        parser_parse_expression(parser);
    }    
}

void parser_parse_expression(parser_t *parser) {
    parser_parse_assignment(parser);
}

void parser_parse_assignment(parser_t *parser) {
    if (parser->current && parser->current->type == TOKEN_IDENTIFIER && parser_peek_token(parser, 1)->type == TOKEN_EQ) {
        parser_expect_advance(parser, TOKEN_IDENTIFIER);
        parser_expect_advance(parser, TOKEN_EQ);
        parser_parse_expression(parser);
    } else {
        parser_parse_logical_or(parser);
    }
}

void parser_parse_logical_or(parser_t *parser) {
    parser_parse_logical_and(parser);
    while (parser_match(parser, TOKEN_OR)) {
        parser_advance(parser);
        parser_parse_logical_and(parser);
    }
}

void parser_parse_logical_and(parser_t *parser) {
    parser_parse_equality(parser);
    while (parser_match(parser, TOKEN_AND)) {
        parser_advance(parser);
        parser_parse_equality(parser);
    }
}

void parser_parse_equality(parser_t *parser) {
    parser_parse_comparision(parser);
    while (parser_match(parser, TOKEN_EQEQ) || parser_match(parser, TOKEN_NEQ)) {
        parser_advance(parser);
        parser_parse_comparision(parser);
    }
}

void parser_parse_comparision(parser_t *parser) {
    parser_parse_term(parser);
    while (parser_match(parser, TOKEN_GT) 
        || parser_match(parser, TOKEN_GEQ)
        || parser_match(parser, TOKEN_LT)
        || parser_match(parser, TOKEN_LEQ)) {
        parser_advance(parser);
        parser_parse_term(parser);
    }
}

void parser_parse_term(parser_t *parser) {
    parser_parse_factor(parser);
    while (parser_match(parser, TOKEN_PLUS) || parser_match(parser, TOKEN_MINUS)) {
        parser_advance(parser);
        parser_parse_factor(parser);
    }
}

void parser_parse_factor(parser_t *parser) {
    parser_parse_unary(parser);
    while (parser_match(parser, TOKEN_ASTERISK) || parser_match(parser, TOKEN_SLASH) || parser_match(parser, TOKEN_PERCENT)) {
        parser_advance(parser);
        parser_parse_unary(parser);
    }
}

void parser_parse_unary(parser_t *parser) {
    if (parser->current->type == TOKEN_NOT) {
        parser_advance(parser);
        parser_parse_unary(parser);
    } else if (parser->current->type == TOKEN_MINUS) {
        parser_advance(parser);
        parser_parse_unary(parser);
    } else {
        parser_parse_primary(parser);
    }
}

void parser_parse_primary(parser_t *parser) {
    if (parser->current->type == TOKEN_LITERAL_INT) {
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_LITERAL_FLOAT) {
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_LITERAL_STR) {
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_TRUE || parser->current->type == TOKEN_FALSE) {
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_NULL) {
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_IDENTIFIER && parser_peek_token(parser, 1)->type == TOKEN_LPAREN) {
        parser_advance(parser);
        parser_expect_advance(parser, TOKEN_LPAREN);
        if (parser->current && parser->current->type != TOKEN_RPAREN) {
            parser_parse_arg_list(parser);
        }
        parser_expect_advance(parser, TOKEN_RPAREN);
    } else if (parser->current->type == TOKEN_IDENTIFIER) {
        parser_advance(parser);
    } else if (parser->current->type == TOKEN_LPAREN) {
        parser_advance(parser);
        parser_parse_expression(parser);
        parser_expect_advance(parser, TOKEN_RPAREN);
    } else {
        fprintf(stderr, "[%ld:%ld] Expected primary expression but got %s\n", parser->current->line, parser->current->column, parser->current->value);
        exit(EXIT_FAILURE);
    }
}

void parser_parse_arg_list(parser_t *parser) {
    parser_parse_expression(parser);
    while (parser_match(parser, TOKEN_COMMA)) {
        parser_advance(parser);
        parser_parse_expression(parser);
    }
}
