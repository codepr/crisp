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

#include "builtins.h"
#include "runtime.h"

#include <stdio.h>


struct expr *builtin_def(Context *ctx, struct expr *exp) {

    if (exp->children[0]->etype != QEXP) {
        expr_err(exp, "Function 'def' passed incorrect types!");
        return exp;
    }

    struct expr *e = exp->children[0];

    for (int i = 0; i < e->count; i++)
        context_put(ctx, e->children[i], exp->children[i + 1]);

    expr_del(exp);

    struct expr *aexp = malloc(sizeof(*aexp));
    expr_sexp(aexp);
    return aexp;
}


struct expr *builtin_len(Context *ctx, struct expr *exp) {

    if (exp->children[0]->etype != QEXP) {
        expr_err(exp, "Function 'len' passed incorrect types!");
        return exp;
    }

    expr_integer(exp, exp->children[0]->count);

    return exp;
}


struct expr *builtin_init(Context *ctx, struct expr *exp) {

    if (exp->children[0]->etype != QEXP) {
        expr_err(exp, "Function 'init' passed incorrect types!");
        return exp;
    }

    if (exp->children[0]->count == 0) {
        expr_err(exp, "function 'init' passed '!");
        return exp;
    }

    /* Otherwise take first argument */
    struct expr *v = expr_take(exp, 0);

    if (expr_peek(v, 0)->etype == SEXP || expr_peek(v, 0)->etype == QEXP)
        v = expr_take(v, 0);

    expr_del(expr_pop(v, 0));

    return v;
}


struct expr *builtin_head(Context *ctx, struct expr *exp) {

    if (exp->children[0]->etype != QEXP) {
        expr_err(exp, "Function 'head' passed incorrect types!");
        return exp;
    }

    if (exp->children[0]->count == 0) {
        expr_err(exp, "function 'head' passed '!");
        return exp;
    }

    /* Otherwise take first argument */
    struct expr *v = expr_take(exp, 0);

    /* if (expr_peek(v, 0)->etype == SEXP || expr_peek(v, 0)->etype == QEXP) */
    /*     v = expr_take(v, 0); */

    /* Delete all elements that are not head and return */
    while (v->count > 1)
        expr_del(expr_pop(v, 1));

    return v;
}


struct expr *builtin_last(Context *ctx, struct expr *exp) {

    if (exp->children[0]->etype != QEXP) {
        expr_err(exp, "Function 'last' passed incorrect types!");
        return exp;
    }

    if (exp->children[0]->count == 0) {
        expr_err(exp, "function 'last' passed '!");
        return exp;
    }

    struct expr *v = expr_take(exp, 0);

    if (expr_peek(v, 0)->etype == SEXP || expr_peek(v, 0)->etype == QEXP)
        v = expr_take(v, 0);

    while (v->count > 1)
        expr_del(expr_pop(v, 0));

    return v;
}


struct expr *builtin_tail(Context *ctx, struct expr *exp) {

    if (exp->children[0]->etype != QEXP) {
        expr_err(exp, "Function 'tail' passed incorrect types!");
        return exp;
    }

    if (exp->children[0]->count == 0) {
        expr_err(exp, "Function 'tail' passed '!");
        return exp;
    }

    struct expr *v = expr_take(exp, 0);

    /* if (expr_peek(v, 0)->etype == SEXP || expr_peek(v, 0)->etype == QEXP) */
    /*     v = expr_take(v, 0); */

    expr_del(expr_pop(v, 0));

    return v;
}


struct expr *builtin_list(Context *ctx, struct expr *exp) {
    exp->etype = QEXP;
    return exp;
}


struct expr *builtin_eval(Context *ctx, struct expr *exp) {

    if (exp->children[0]->etype != QEXP) {
        expr_err(exp, "Function 'eval' passed incorrect types!");
        return exp;
    }

    struct expr *x = expr_take(expr_take(exp, 0), 0);
    /* struct expr *x = expr_take(exp, 0); */
    x->etype = SEXP;

    return eval(ctx, x);
}


struct expr *builtin_integer_op(struct expr *exp, char operator,
                                long long num1, long long num2) {

    switch (operator) {
        case '+':
            expr_integer(exp, num1 + num2);
            break;
        case '-':
            expr_integer(exp, num1 - num2);
            break;
        case '*':
            expr_integer(exp, num1 * num2);
            break;
        case '/':
            if (num2 == 0) {
                char err[MAX_ERR_SIZE];
                sprintf(err, "%s -> %lld / %lld", ERR_DIV_BY_ZERO, num1, num2);
                expr_err(exp, err);
            }
            else
                expr_integer(exp, num1 / num2);
            break;
        case '%':
            expr_integer(exp, num1 % num2);
            break;
        default:
            expr_err(exp, ERR_INVALID_INT_OP);
            break;
    }

    return exp;
}


struct expr *builtin_decimal_op(struct expr *exp, char operator,
                                double num1, double num2) {

    switch (operator) {
        case '+':
            expr_decimal(exp, num1 + num2);
            break;
        case '-':
            expr_decimal(exp, num1 - num2);
            break;
        case '*':
            expr_decimal(exp, num1 * num2);
            break;
        case '/':
            if (num2 == 0.0000) {
                char err[MAX_ERR_SIZE];
                sprintf(err, "%s -> %lf / %lf", ERR_DIV_BY_ZERO, num1, num2);
                expr_err(exp, err);
            }
            else
                expr_decimal(exp, num1 / num2);
            break;
        default:
            expr_err(exp, ERR_INVALID_DEC_OP);
            break;
    }

    return exp;
}
