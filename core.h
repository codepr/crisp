/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2018, Andrea Giacomo Baldan All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <string.h>


#define ZLISP_VERSION       "0.0.1"
#define MAX_SYM_SIZE        16
#define MAX_ERR_SIZE        64
#define ERR_UNDEFINED_SYM   "Undefined symbol"
#define ERR_DIV_BY_ZERO     "Division by zero"
#define ERR_INVALID_INT_OP  "Invalid operation between integers"
#define ERR_INVALID_DEC_OP  "Invalid operation between decimals"


typedef enum {
    SEXP,
    SEXP_END,
    QEXP,
    FUNCTION,
    INTEGER,
    DECIMAL,
    SYMBOL,
    STRING,
    ERROR
} extype;


struct context {
    int count;
    int capacity;
    struct function {
        char *symbol;
        struct expr *exp;
    } **funcs;
};


typedef struct expr fun(struct context *, struct expr *);


struct expr {
    extype etype;
    union {
        struct {
            struct expr **children;
            int count;
            int capacity;
        };
        char *string;
        char symbol[MAX_SYM_SIZE];
        char err[MAX_ERR_SIZE];
        long long integer;
        double decimal;
        fun *fn;
    };
};


void context_init(struct context *);

void expr_string(struct expr *, char *);

void expr_integer(struct expr *, long long);

void expr_decimal(struct expr *, double);

void expr_operator(struct expr *, char);

void expr_symbol(struct expr *, char *);

void expr_sexp(struct expr *);

void expr_qexp(struct expr *);

void expr_end(struct expr *);

void expr_err(struct expr *, char *);

void expr_fun(struct expr *, fun *);

struct expr *expr_append(struct expr *, struct expr *);

struct expr *expr_peek(struct expr *, int);

struct expr *expr_pop(struct expr *, int);

void expr_del(struct expr *);

struct expr *expr_take(struct expr *, int);


#endif
