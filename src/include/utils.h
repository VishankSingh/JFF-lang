/**
 * File Name: utils.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_MEM_ALLOC_ERROR(ptr) \
    do { \
        if (!(ptr)) { \
            fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CHECK_NULL_ERROR(ptr) \
    do { \
        if (!(ptr)) { \
            fprintf(stderr, "Null pointer error at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CHECK_FILE_ERROR(file) \
    do { \
        if (!(file)) { \
            fprintf(stderr, "File error at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CHECK_CONDITION(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s at %s:%d\n", (message), __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
    

#endif // UTILS_H