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

#include "runtime.h"
#include "builtins.h"

#include <stdio.h>

#define IS_SPACE(c)     (c == ' ' || c == '\n')


struct runtime {
    struct context *ctx;
};


static struct runtime runtime = {
    .ctx = &(struct context) {0}
};


static struct expr *compute_op(struct expr *exp, char operator) {

    for (int i = 0; i < exp->count; i++) {
        if (exp->children[i]
            && exp->children[i]->etype != INTEGER
            && exp->children[i]->etype != DECIMAL
            && exp->children[i]->etype != ERROR
            && exp->children[i]->etype != SEXP_END) {
            expr_del(exp);
            return NULL;
        }
    }

    struct expr *x = expr_pop(exp, 0);

    if (operator == '-' && exp->count == 2)
        x->integer = -x->integer;

    while (exp->count > 0) {

        struct expr *y = expr_pop(exp, 0);

        if (!y || y->etype == SEXP_END)
            continue;

        if (x->etype == DECIMAL || y->etype == DECIMAL)
            x = builtin_decimal_op(x, operator,
                                   x->etype == DECIMAL ?
                                   x->decimal : x->integer,
                                   y->etype == DECIMAL ?
                                   y->decimal : y->integer);
        else
            x = builtin_integer_op(x, operator, x->integer, y->integer);

        if (x->etype == ERROR)
            return x;

        if (y->etype == ERROR)
            return y;

        expr_del(y);
    }

    expr_del(exp);

    return x;
}


static struct expr *compute(struct expr *exp, char *symbol) {

    if (strcmp("len", symbol) == 0)
        return builtin_len(exp);
    if (strcmp("head", symbol) == 0)
        return builtin_head(exp);
    if (strcmp("last", symbol) == 0)
        return builtin_last(exp);
    if (strcmp("init", symbol) == 0)
        return builtin_init(exp);
    if (strcmp("eval", symbol) == 0)
        return builtin_eval(exp);
    if (strcmp("tail", symbol) == 0)
        return builtin_tail(exp);
    if (strcmp("list", symbol) == 0)
        return builtin_list(exp);
    if (strstr("+-*/%", symbol))
        return compute_op(exp, *symbol);

    expr_err(exp, ERR_UNDEFINED_SYM);

    return exp;
}


static struct expr *expr_eval(struct expr *exp) {

    for (int i = 0; i < exp->count; i++)
        exp->children[i] = eval(exp->children[i]);

    for (int i = 0; i < exp->count; i++)
        if (exp->children[i] && exp->children[i]->etype == ERROR)
            return expr_take(exp, i);

    if (exp->count == 0)
        return exp;

    if (exp->count == 1)
        return expr_take(exp, 0);

    struct expr *sxp = expr_pop(exp, 0);

    if (sxp->etype != SYMBOL) {
        expr_del(sxp);
        expr_err(exp, "Not a symbol");
        return exp;
    }

    struct expr *result = compute(exp, sxp->symbol);
    expr_del(sxp);

    return result;
}


struct expr *eval(struct expr *exp) {

    if (exp && exp->etype == SEXP)
        return expr_eval(exp);

    return exp;
}


static inline void banner(void) {
    printf("\nStart zlisp REPL v%s\n", ZLISP_VERSION);
    printf("Press Ctrl+c to exit\n\n");
    printf("zlisp> ");
}


static void expr_print(struct expr *exp) {

    if (!exp)
        return;

    switch (exp->etype) {
        case SEXP:
            printf("(");
            for (int i = 0; i < exp->count; i++)
                expr_print(exp->children[i]);
            printf(")");
            break;
        case QEXP:
            printf("'");
            for (int i = 0; i < exp->count; i++)
                expr_print(exp->children[i]);
            break;
        case INTEGER:
            printf("%lld ", exp->integer);
            break;
        case DECIMAL:
            printf("%lf ", exp->decimal);
            break;
        case SYMBOL:
            printf("%s ", exp->symbol);
            break;
        case STRING:
            printf("\"%s\" ", exp->string);
            break;
        case ERROR:
            printf("Error: %s", exp->err);
            break;
        case SEXP_END:
            break;
        default:
            break;
    }
}


static struct expr *parse_expr(char **buf) {

    if (!**buf)
        return NULL;

    while (IS_SPACE(**buf)) (*buf)++;

    if (!**buf)
        return NULL;

    struct expr *exp = malloc(sizeof(*exp));

    if (**buf == '(' || **buf == '\'') {
        expr_sexp(exp);
        if (**buf == '\'')
            exp->etype = QEXP;
        (*buf)++;
        while (**buf && **buf != ')')
            exp = expr_append(exp, parse_expr(buf));
        (*buf)++;
    } else if (**buf == ')') {
        expr_end(exp);
        (*buf)++;
    } else if (**buf == '+' || **buf == '-' || **buf == '*'
               || **buf == '/' || **buf == '%') {
        expr_operator(exp, **buf);
        (*buf)++;
    } else if ('0' <= **buf && **buf <= '9') {
        char tmp[MAX_ERR_SIZE];
        int decimal = 0;
        int i = 0;
        tmp[i++] = **buf;
        (*buf)++;
        while (**buf && (('0' <= **buf && **buf <= '9') || **buf == '.')) {
            if (**buf == '.')
                decimal = 1;
            tmp[i++] = **buf;
            (*buf)++;
        }
        tmp[i] = '\0';

        long long n;
        double d;
        if (decimal == 0 && sscanf(tmp, "%lld", &n))
            expr_integer(exp, n);
        else if (sscanf(tmp, "%lf", &d))
            expr_decimal(exp, d);
    } else if (**buf == '"') {
        int base_size = MAX_SYM_SIZE;
        char *str = malloc(base_size * sizeof(char));
        int i = 0;
        (*buf)++;
        while (**buf != '"') {
            if (i == base_size) {
                base_size *= 2;
                str = realloc(str, base_size * sizeof(char));
            }
            str[i] = **buf;
            (*buf)++;
            i++;
        }
        str[i] = '\0';
        (*buf)++;
        expr_string(exp, str);

    } else {

        if (strncmp("head", *buf, 4) == 0) {
            expr_symbol(exp, "head");
            (*buf) += 4;
        } else if (strncmp("len", *buf, 3) == 0) {
            expr_symbol(exp, "len");
            (*buf) += 3;
        } else if (strncmp("last", *buf, 4) == 0) {
            expr_symbol(exp, "last");
            (*buf) += 4;
        } else if (strncmp("init", *buf, 4) == 0) {
            expr_symbol(exp, "init");
            (*buf) += 4;
        } else if (strncmp("eval", *buf, 4) == 0) {
            expr_symbol(exp, "eval");
            (*buf) += 4;
        } else if (strncmp("tail", *buf, 4) == 0) {
            expr_symbol(exp, "tail");
            (*buf) += 4;
        } else if (strncmp("list", *buf, 4) == 0) {
            expr_symbol(exp, "list");
            (*buf) += 4;
        } else {
            expr_err(exp, "Unknown token");
            (*buf)++;
        }
    }

    return exp;
}


static struct expr *parse(char *buf) {

    struct expr *exp = malloc(sizeof(*exp));
    expr_sexp(exp);

    if (*buf == '(') {
        buf++;
    } else if (*buf == '\'') {
        exp->etype = QEXP;
        buf++;
    }

    while (*buf)
        expr_append(exp, parse_expr(&buf));

    return exp;
}


int main(void) {

    char buf[256];

    banner();

    context_init(runtime.ctx);

    while (fgets(buf, 256, stdin)) {

        if (strlen(buf) < 2) {
            printf("zlisp> ");
            continue;
        }

        struct expr *exp = parse(buf);

        expr_print(exp);
        printf("\n");

        if (expr_peek(exp, 0)->etype != SYMBOL) {
            expr_print(exp);
        } else {
            struct expr *sxp = eval(exp);

            expr_print(sxp);

            expr_del(sxp);
        }

        printf("\nzlisp> ");

        memset(buf, 0x00, 256);

    }

    return 0;
}
