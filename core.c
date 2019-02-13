/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2019, Andrea Giacomo Baldan All rights reserved.
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

#include "core.h"


void context_init(Context *ctx) {
    hashtable_init(ctx, NULL);
}


void context_release(Context *ctx) {
    hashtable_release(ctx);
}


int context_put(Context *ctx, struct expr *esym, struct expr *efun) {
    printf("adding symbol to context %s\n", esym->symbol);
    return hashtable_put(ctx, esym->symbol, expr_copy(efun));
}


struct expr *context_get(Context *ctx, struct expr *exp) {
    struct expr *e = hashtable_get(ctx, exp->symbol);
    if (!e) {
        struct expr *err = malloc(sizeof(*err));
        expr_err(err, "Unbound symbol");
        return err;
    }
    return expr_copy(e);
}


int context_del(Context *ctx, struct expr *exp) {
    return hashtable_del(ctx, exp->symbol);
}


void expr_string(struct expr *exp, char *str) {
    exp->etype = STRING;
    exp->string = str;
}


void expr_integer(struct expr *exp, long long x) {
    exp->etype = INTEGER;
    exp->integer = x;
}


void expr_decimal(struct expr *exp, double x) {
    exp->etype = DECIMAL;
    exp->decimal = x;
}


void expr_operator(struct expr *exp, char op) {
    exp->etype = SYMBOL;
    exp->symbol[0] = op;
    exp->symbol[1] = '\0';
}


void expr_symbol(struct expr *exp, char *sym) {
    exp->etype = SYMBOL;
    strcpy(exp->symbol, sym);
}


void expr_sexp(struct expr *exp) {
    exp->etype = SEXP;
    exp->count = 0;
    exp->capacity = 4;
    exp->children = malloc(exp->capacity * sizeof(struct expr *));
}


void expr_qexp(struct expr *exp) {
    exp->etype = QEXP;
    exp->count = 0;
    exp->capacity = 4;
    exp->children = malloc(exp->capacity * sizeof(struct expr *));
}


void expr_end(struct expr *exp) {
    exp->etype = SEXP_END;
}


void expr_err(struct expr *exp, char *err) {
    exp->etype = ERROR;
    strncpy(exp->err, err, MAX_ERR_SIZE);
    exp->err[MAX_ERR_SIZE - 1] = '\0';
}


void expr_fun(struct expr *exp, fun *fn) {
    exp->etype = FUNCTION;
    exp->fn = fn;
}


struct expr *expr_append(struct expr *exp, struct expr *nexp) {
    if (exp->count + 1 >= exp->capacity / 2) {
        exp->capacity *= 2;
        exp->children = realloc(exp->children,
                                  exp->capacity * sizeof(struct expr *));
    }
    exp->children[exp->count++] = nexp;
    return exp;
}


struct expr *expr_peek(struct expr *v, int i) {
    return v->children[i];
}


struct expr *expr_pop(struct expr *v, int i) {
    /* Find the item at "i" */
    struct expr *x = v->children[i];

    /* Shift memory after the item at "i" over the top */
    memmove(&v->children[i], &v->children[i+1],
            sizeof(struct expr *) * (v->count-i-1));

    /* Decrease the count of items in the list */
    v->count--;

    /* Reallocate the memory used */
    if (v->count < v->capacity / 3) {
        v->capacity /= 2;
        v->children = realloc(v->children, sizeof(struct expr *) * v->capacity);
    }

    return x;
}


void expr_del(struct expr *v) {

    if (!v)
        return;

    switch (v->etype) {

        /* If exprr then delete all elements inside */
        case QEXP:
        case SEXP:
            for (int i = 0; i < v->count; i++)
                expr_del(v->children[i]);

            /* Also free the memory allocated to contain the pointers */
            free(v->children);

            break;
        case STRING:
            free(v->string);
            break;
        default:
            break;
    }

    /* Free the memory allocated for the "expr" struct itself */
    free(v);
}


struct expr *expr_take(struct expr *v, int i) {
    struct expr *x = expr_pop(v, i);
    expr_del(v);
    return x;
}


struct expr *expr_copy(struct expr *exp) {

    if (!exp)
        return NULL;

    struct expr *x = malloc(sizeof(*x));
    x->etype = exp->etype;

    switch (exp->etype) {
        case FUNCTION:
            x->fn = exp->fn;
            break;
        case INTEGER:
            x->integer = exp->integer;
            break;
        case DECIMAL:
            x->decimal = exp->decimal;
            break;
        case SYMBOL:
            strcpy(x->symbol, exp->symbol);
            break;
        case SEXP:
        case QEXP:
            x->count = exp->count;
            x->children = malloc(x->count * sizeof(struct expr *));
            for (int i = 0; i < x->count; i++)
                x->children[i] = expr_copy(exp->children[i]);
            break;
        case ERROR:
            strcpy(x->err, exp->err);
            break;
        case STRING:
            x->string = malloc(strlen(exp->string) + 1);
            strcpy(x->string, exp->string);
            break;
        default:
            break;
    }

    return x;
}
