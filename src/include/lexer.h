/**
 * File Name: lexer.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    LEXER_SUCCESS = 0,
    LEXER_ERROR = -1,
    LEXER_EOF = -2,
    LEXER_INVALID_TOKEN = -3
} lexer_status_t;

typedef enum {
    TOKEN_KEYWORD,
    // keywords
    TOKEN_FUNC,
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_RETURN,
    TOKEN_PRINT,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_NULL,
    TOKEN_TRUE,
    TOKEN_FALSE,
    

    // identifiers and literals
    TOKEN_IDENTIFIER,
    TOKEN_LITERAL_INT,
    TOKEN_LITERAL_FLOAT,
    TOKEN_LITERAL_STR,

    // types
    TOKEN_TYPE_INT,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_BOOL,
    TOKEN_TYPE_VOID,

    // operators
    TOKEN_EQ,       // =
    TOKEN_EQEQ,      // ==
    TOKEN_NEQ,      // !=
    TOKEN_PLUS,     // +
    TOKEN_MINUS,    // -
    TOKEN_ASTERISK, // *
    TOKEN_SLASH,    // /
    TOKEN_GT,       // >
    TOKEN_GEQ,      // >=
    TOKEN_LT,       // <
    TOKEN_LEQ,      // <=
    TOKEN_PERCENT,  // %
    TOKEN_AND,      // &&
    TOKEN_OR,       // ||
    TOKEN_NOT,      // !

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,

    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_INVALID
} token_type_t;

typedef struct TOKEN_STRUCT {
    token_type_t type;
    char *value;
    size_t length;
    size_t line;
    size_t column;
} token_t;

token_t *init_token(token_type_t type, const char *value, size_t length, size_t line, size_t column);
void free_token(token_t *token);
void print_token(token_t *token);
void print_token_type(token_type_t type);
void print_token_value(token_t *token);

char *token_type_to_string(token_type_t type);

typedef struct LEXER_STRUCT {
    char *filename;
    char *input;
    size_t position;
    size_t read_position;
    char current_char;
    lexer_status_t status;

    size_t line;
    size_t column;

    token_t current_token;

    size_t token_count;
    size_t tokens_capacity;
    token_t **tokens;
} lexer_t;

lexer_t *init_lexer(const char *filename);
void free_lexer(lexer_t *lexer);

token_t *lexer_next_token(lexer_t *lexer);
void lexer_advance(lexer_t *lexer);
char lexer_peek(lexer_t *lexer);


void lexer_skip_whitespace(lexer_t *lexer);
void lexer_skip_comment(lexer_t *lexer);
void lexer_append_token(lexer_t *lexer, token_t *token);

#endif // LEXER_H