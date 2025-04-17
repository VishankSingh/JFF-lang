/**
 * File Name: lexer.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/lexer.h"
#include "include/utils.h"

token_t *init_token(token_type_t type, const char *value, size_t length, size_t line, size_t column) {
    token_t *token = malloc(sizeof(token_t));
    CHECK_MEM_ALLOC_ERROR(token);
    token->type = type;
    token->length = length;
    token->value = malloc(length + 1);
    CHECK_MEM_ALLOC_ERROR(token->value);
    memcpy(token->value, value, length);
    token->value[length] = '\0';
    token->line = line;
    token->column = column;
    return token;
}

void free_token(token_t *token) {
    if (token) {
        free(token->value);
        token->value = NULL;
        free(token);
        token = NULL;
    }
}

void print_token(token_t *token) {
    if (token) {
        printf("(%2ld:%-2ld | ", token->line, token->column);
        print_token_type(token->type);
        printf(" | %-15s )\n", token->value ? token->value : "NULL");
    }
}

void print_token_type(token_type_t type) {
    switch (type) {
        case TOKEN_KEYWORD:         printf("KEYWORD       "); break;
        case TOKEN_FUNC:            printf("FUNC          "); break;
        case TOKEN_IF:              printf("IF            "); break;
        case TOKEN_ELIF:            printf("ELIF          "); break;
        case TOKEN_ELSE:            printf("ELSE          "); break;
        case TOKEN_RETURN:          printf("RETURN        "); break;
        case TOKEN_PRINT:           printf("PRINT         "); break;
        case TOKEN_FOR:             printf("FOR           "); break;
        case TOKEN_NULL:            printf("NULL          "); break;
        case TOKEN_TRUE:            printf("TRUE          "); break;
        case TOKEN_FALSE:           printf("FALSE         "); break;

        case TOKEN_IDENTIFIER:      printf("IDENTIFIER    "); break;
        case TOKEN_LITERAL_INT:     printf("INT LITERAL   "); break;
        case TOKEN_LITERAL_FLOAT:   printf("FLOAT LITERAL "); break;
        case TOKEN_LITERAL_STR:     printf("STRING LITERAL"); break;

        case TOKEN_TYPE_INT:        printf("TYPE INT      "); break;
        case TOKEN_TYPE_FLOAT:      printf("TYPE FLOAT    "); break;
        case TOKEN_TYPE_STRING:     printf("TYPE STRING   "); break;
        case TOKEN_TYPE_BOOL:       printf("TYPE BOOL     "); break;
        case TOKEN_TYPE_VOID:       printf("TYPE VOID     "); break;
        
        case TOKEN_EQ:              printf("EQUAL         "); break;
        case TOKEN_EQEQ:             printf("DOUBLE EQUAL  "); break;
        case TOKEN_NEQ:             printf("NOT EQUAL     "); break;
        case TOKEN_PLUS:            printf("PLUS          "); break;
        case TOKEN_MINUS:           printf("MINUS         "); break;
        case TOKEN_ASTERISK:        printf("ASTERISK      "); break;
        case TOKEN_SLASH:           printf("SLASH         "); break;
        case TOKEN_PERCENT:         printf("PERCENT       "); break;
        case TOKEN_LT:              printf("LESS THAN     "); break;
        case TOKEN_GT:              printf("GREATER THAN  "); break;
        case TOKEN_AND:             printf("AND           "); break;
        case TOKEN_OR:              printf("OR            "); break;
        case TOKEN_NOT:             printf("NOT           "); break;

        case TOKEN_LPAREN:          printf("LPAREN        "); break;
        case TOKEN_RPAREN:          printf("RPAREN        "); break;
        case TOKEN_LBRACE:          printf("LBRACE        "); break;
        case TOKEN_RBRACE:          printf("RBRACE        "); break;
        case TOKEN_SEMICOLON:      printf("SEMINCOLON    "); break;
        case TOKEN_COLON:           printf("COLON         "); break;
        case TOKEN_COMMA:           printf("COMMA         "); break;

        case TOKEN_INVALID:         printf("INVALID       "); break;
        case TOKEN_UNKNOWN:         printf("UNKNOWN       "); break;
        case TOKEN_EOF:             printf("EOF           "); break;
        default:                    printf("UNKNOWN       ");
    }
}

void print_token_value(token_t *token) {
    if (token && token->value) {
        printf("%s", token->value);
    } else {
        printf("NULL");
    }
}

char *token_type_to_string(token_type_t type) {
    switch (type) {
        case TOKEN_KEYWORD:         return "KEYWORD";
        case TOKEN_FUNC:            return "FUNC";
        case TOKEN_IF:              return "IF";
        case TOKEN_ELIF:            return "ELIF";
        case TOKEN_ELSE:            return "ELSE";
        case TOKEN_RETURN:          return "RETURN";
        case TOKEN_PRINT:           return "PRINT";
        case TOKEN_FOR:             return "FOR";
        case TOKEN_NULL:            return "NULL";
        case TOKEN_TRUE:            return "TRUE";
        case TOKEN_FALSE:           return "FALSE";
        case TOKEN_IDENTIFIER:      return "IDENTIFIER";
        case TOKEN_LITERAL_INT:     return "INT LITERAL";
        case TOKEN_LITERAL_FLOAT:   return "FLOAT LITERAL";
        case TOKEN_LITERAL_STR:     return "STRING LITERAL";
        case TOKEN_TYPE_INT:        return "TYPE INT";
        case TOKEN_TYPE_FLOAT:      return "TYPE FLOAT";
        case TOKEN_TYPE_STRING:     return "TYPE STRING";
        case TOKEN_TYPE_BOOL:       return "TYPE BOOL";
        case TOKEN_TYPE_VOID:       return "TYPE VOID";
        case TOKEN_EQ:              return "EQUAL (=)";
        case TOKEN_EQEQ:            return "EQUAL EQUAL (==)";
        case TOKEN_NEQ:             return "NOT EQUAL (!=)";
        case TOKEN_PLUS:            return "PLUS (+)";
        case TOKEN_MINUS:           return "MINUS (-)";
        case TOKEN_ASTERISK:        return "ASTERISK (*)";
        case TOKEN_SLASH:           return "SLASH (/)";
        case TOKEN_GT:              return "GREATER THAN (>)";
        case TOKEN_GEQ:             return "GREATER EQUAL (>=)";
        case TOKEN_LT:              return "LESS THAN (<)";
        case TOKEN_LEQ:             return "LESS EQUAL (<=)";
        case TOKEN_PERCENT:         return "MODULO (%)";
        case TOKEN_AND:             return "AND (&&)";
        case TOKEN_OR:              return "OR (||)";
        case TOKEN_NOT:             return "NOT (!)";
        case TOKEN_LPAREN:          return "LEFT PAREN";
        case TOKEN_RPAREN:          return "RIGHT PAREN";
        case TOKEN_LBRACE:          return "LEFT BRACE";
        case TOKEN_RBRACE:          return "RIGHT BRACE";
        case TOKEN_SEMICOLON:       return "SEMICOLON";
        case TOKEN_COLON:           return "COLON";
        case TOKEN_COMMA:           return "COMMA";
        case TOKEN_EOF:             return "EOF";
        case TOKEN_UNKNOWN:         return "UNKNOWN";
        case TOKEN_INVALID:         return "INVALID";

        default: {
            static char buf[32];
            snprintf(buf, sizeof(buf), "UNKNOWN(%d)", type);
            return buf;
        }
    }
}


lexer_t *init_lexer(const char *filename) {
    lexer_t *lexer = malloc(sizeof(lexer_t));
    CHECK_MEM_ALLOC_ERROR(lexer);
    lexer->filename = strdup(filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        free(lexer);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    lexer->input = malloc(file_size + 1);
    CHECK_MEM_ALLOC_ERROR(lexer->input);
    fread(lexer->input, 1, file_size, file);
    lexer->input[file_size] = '\0';
    fclose(file);

    lexer->position = 0;
    lexer->read_position = 0;
    lexer->current_char = '\0';
    lexer->status = LEXER_SUCCESS;

    lexer->line = 1;
    lexer->column = 0;

    lexer->current_token.type = TOKEN_EOF;
    lexer->current_token.value = NULL;
    lexer->current_token.length = 0;

    lexer->token_count = 0;
    lexer->tokens_capacity = 10;
    lexer->tokens = malloc(lexer->tokens_capacity * sizeof(token_t *));
    CHECK_MEM_ALLOC_ERROR(lexer->tokens);

    lexer_advance(lexer);
    return lexer;
}

void free_lexer(lexer_t *lexer) {
    if (lexer) {
        free(lexer->input);
        for (size_t i = 0; i < lexer->token_count; i++) {
            free_token(lexer->tokens[i]);
            lexer->tokens[i] = NULL;
        }
        free(lexer->filename);
        lexer->filename = NULL;
        free(lexer->tokens);
        lexer->tokens = NULL;
        free(lexer);
        lexer = NULL;
    }
}

void lexer_advance(lexer_t *lexer) {
    lexer->position = lexer->read_position;

    if (lexer->read_position >= strlen(lexer->input)) {
        lexer->current_char = '\0';
    } else {
        lexer->current_char = lexer->input[lexer->read_position];

        if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 0;
        } else {
            lexer->column++;
        }
    }

    lexer->read_position++;
}


char lexer_peek(lexer_t *lexer) {
    return lexer->read_position >= strlen(lexer->input) ? '\0' : lexer->input[lexer->read_position];
}

void lexer_skip_whitespace(lexer_t *lexer) {
    while (lexer->current_char == ' ' 
        || lexer->current_char == '\t' 
        || lexer->current_char == '\r'
        || lexer->current_char == '\n') {
        lexer_advance(lexer);
    }
}

void lexer_apend_token(lexer_t *lexer, token_t *token) {
    if (lexer->token_count >= lexer->tokens_capacity) {
        lexer->tokens_capacity *= 2;
        lexer->tokens = (token_t **)realloc(lexer->tokens, lexer->tokens_capacity * sizeof(token_t));
    }
    lexer->tokens[lexer->token_count++] = token;
}

token_t *lexer_next_token(lexer_t *lexer) {
    lexer_skip_whitespace(lexer);

    while (lexer->current_char != '\0') {
        // identifier or keyword
        if (isalpha(lexer->current_char) || lexer->current_char == '_') {
            size_t start = lexer->position;
            size_t start_line = lexer->line;
            size_t start_column = lexer->column;
            while (isalnum(lexer->current_char) || lexer->current_char == '_') {
                lexer_advance(lexer);
            }
            size_t length = lexer->position - start;


            // Check for keywords
            if (length == 4 && strncmp(lexer->input + start, "func", length) == 0) {
                return init_token(TOKEN_FUNC, "func", length, start_line, start_column);
            }
            else if (length == 2 && strncmp(lexer->input + start, "if", length) == 0) {
                return init_token(TOKEN_IF, "if", length, start_line, start_column);
            } 
            else if (length == 4 && strncmp(lexer->input + start, "elif", length) == 0) {
                return init_token(TOKEN_ELIF, "elif", length, start_line, start_column);
            } 
            else if (length == 4 && strncmp(lexer->input + start, "else", length) == 0) {
                return init_token(TOKEN_ELSE, "else", length, start_line, start_column);
            } 
            else if (length == 6 && strncmp(lexer->input + start, "return", length) == 0) {
                return init_token(TOKEN_RETURN, "return", length, start_line, start_column);
            } 
            else if (length == 5 && strncmp(lexer->input + start, "print", length) == 0) {
                return init_token(TOKEN_PRINT, "print", length, start_line, start_column);
            } 
            else if (length == 3 && strncmp(lexer->input + start, "for", length) == 0) {
                return init_token(TOKEN_FOR, "for", length, start_line, start_column);
            }
            else if (length == 5 && strncmp(lexer->input + start, "while", length) == 0) {
                return init_token(TOKEN_WHILE, "while", length, start_line, start_column);
            } 
            else if (length == 5 && strncmp(lexer->input + start, "break", length) == 0) {
                return init_token(TOKEN_BREAK, "break", length, start_line, start_column);
            } 
            else if (length == 8 && strncmp(lexer->input + start, "continue", length) == 0) {
                return init_token(TOKEN_CONTINUE, "continue", length, start_line, start_column);
            } 
            else if (length == 4 && strncmp(lexer->input + start, "null", length) == 0) {
                return init_token(TOKEN_NULL, "null", length, start_line, start_column);
            }
            else if (length == 4 && strncmp(lexer->input + start, "true", length) == 0) {
                return init_token(TOKEN_TRUE, "true", length, start_line, start_column);
            } 
            else if (length == 5 && strncmp(lexer->input + start, "false", length) == 0) {
                return init_token(TOKEN_FALSE, "false", length, start_line, start_column);
            } 


            // types keywords
            if (length == 3 && strncmp(lexer->input + start, "int", length) == 0) {
                return init_token(TOKEN_TYPE_INT, "int", length, start_line, start_column);
            } 
            else if (length == 5 && strncmp(lexer->input + start, "float", length) == 0) {
                return init_token(TOKEN_TYPE_FLOAT, "float", length, start_line, start_column);
            } 
            else if (length == 6 && strncmp(lexer->input + start, "string", length) == 0) {
                return init_token(TOKEN_TYPE_STRING, "string", length, start_line, start_column);
            } 
            else if (length == 4 && strncmp(lexer->input + start, "bool", length) == 0) {
                return init_token(TOKEN_TYPE_BOOL, "bool", length, start_line, start_column);
            } 
            else if (length == 4 && strncmp(lexer->input + start, "void", length) == 0) {
                return init_token(TOKEN_TYPE_VOID, "void", length, start_line, start_column);
            }


            return init_token(TOKEN_IDENTIFIER, lexer->input + start, length, start_line, start_column);
        }

        if (isdigit(lexer->current_char) || ((lexer->current_char == '-' || lexer->current_char == '+') && isdigit(lexer_peek(lexer)))) {
            size_t start = lexer->position;
            size_t start_line = lexer->line;
            size_t start_column = lexer->column;

            if (lexer->current_char == '-' || lexer->current_char == '+') {
                lexer_advance(lexer);
            }

            while (isdigit(lexer->current_char)) {
                lexer_advance(lexer);
            }
            size_t length = lexer->position - start;
            return init_token(TOKEN_LITERAL_INT, lexer->input + start, length, start_line, start_column);
        }

        if (lexer->current_char == '"') {
            lexer_advance(lexer);
            size_t start = lexer->position;
            size_t start_line = lexer->line;
            size_t start_column = lexer->column;

            while (lexer->current_char != '"' && lexer->current_char != '\0') {
                if (lexer->current_char == '\\') {
                    lexer_advance(lexer);
                }
                lexer_advance(lexer);
            }

            if (lexer->current_char == '"') {
                size_t length = lexer->position - start;
                lexer_advance(lexer); 
                return init_token(TOKEN_LITERAL_STR, lexer->input + start, length, start_line, start_column);
            } else {
                return init_token(TOKEN_INVALID, "Unterminated string", 18, start_line, start_column);
            }
        }

        // Handle single-char tokens
        char ch = lexer->current_char;
        size_t start_line = lexer->line;
        size_t start_column = lexer->column;
        if (ch == '=' && lexer_peek(lexer) == '=') {
            lexer_advance(lexer);
            lexer_advance(lexer);
            return init_token(TOKEN_EQEQ, "==", 2, start_line, start_column);
        } else if (ch == '!' && lexer_peek(lexer) == '=') {
            lexer_advance(lexer);
            lexer_advance(lexer);
            return init_token(TOKEN_NEQ, "!=", 2, start_line, start_column);
        } else if (ch == '&' && lexer_peek(lexer) == '&') {
            lexer_advance(lexer);
            lexer_advance(lexer);
            return init_token(TOKEN_AND, "&&", 2, start_line, start_column);
        } else if (ch == '|' && lexer_peek(lexer) == '|') {
            lexer_advance(lexer);
            lexer_advance(lexer);
            return init_token(TOKEN_OR, "||", 2, start_line, start_column);
        } else if (ch == '!' && lexer_peek(lexer) != '=') {
            return init_token(TOKEN_NOT, "!", 1, start_line, start_column);
        } else if (ch == '<' && lexer_peek(lexer) == '=') {
            lexer_advance(lexer);
            lexer_advance(lexer);
            return init_token(TOKEN_LEQ, "<=", 2, start_line, start_column);
        } else if (ch == '>' && lexer_peek(lexer) == '=') {
            lexer_advance(lexer);
            lexer_advance(lexer);
            return init_token(TOKEN_GEQ, ">=", 2, start_line, start_column);
        }

        lexer_advance(lexer);
        switch (ch) {
            case '+': return init_token(TOKEN_PLUS, "+", 1, start_line, start_column);
            case '-': return init_token(TOKEN_MINUS, "-", 1, start_line, start_column);
            case '*': return init_token(TOKEN_ASTERISK, "*", 1, start_line, start_column);
            case '/': return init_token(TOKEN_SLASH, "/", 1, start_line, start_column);
            case '%': return init_token(TOKEN_PERCENT, "%", 1, start_line, start_column);
            case '=': return init_token(TOKEN_EQ, "=", 1, start_line, start_column);
            case '<': return init_token(TOKEN_LT, "<", 1, start_line, start_column);
            case '>': return init_token(TOKEN_GT, ">", 1, start_line, start_column);
            case '(': return init_token(TOKEN_LPAREN, "(", 1, start_line, start_column);
            case ')': return init_token(TOKEN_RPAREN, ")", 1, start_line, start_column);
            case '{': return init_token(TOKEN_LBRACE, "{", 1, start_line, start_column);
            case '}': return init_token(TOKEN_RBRACE, "}", 1, start_line, start_column);
            case ';': return init_token(TOKEN_SEMICOLON, ";", 1, start_line, start_column);
            case ':': return init_token(TOKEN_COLON, ":", 1, start_line, start_column);
            case ',': return init_token(TOKEN_COMMA, ",", 1, start_line, start_column);
            default: {
                char unknown[2] = {ch, '\0'};
                return init_token(TOKEN_INVALID, unknown, 1, start_line, start_column);
            }
        }
    }

    return init_token(TOKEN_EOF, "EOF", 3, lexer->line, lexer->column);
}



