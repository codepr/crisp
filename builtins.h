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

#ifndef BUILTINS_H
#define BUILTINS_H

#include "core.h"


struct expr *builtin_def(Context *, struct expr *);

struct expr *builtin_len(Context *, struct expr *);

struct expr *builtin_init(Context *, struct expr *);

struct expr *builtin_head(Context *, struct expr *);

struct expr *builtin_last(Context *, struct expr *);

struct expr *builtin_tail(Context *, struct expr *);

struct expr *builtin_list(Context *, struct expr *);

struct expr *builtin_eval(Context *, struct expr *);

struct expr *builtin_integer_abs(struct expr *, long long);

struct expr *builtin_integer_sqrt(struct expr *, long long);

struct expr *builtin_decimal_abs(struct expr *, double);

struct expr *builtin_decimal_sqrt(struct expr *, double);

struct expr *builtin_integer_op(struct expr *, char, long long, long long);

struct expr *builtin_decimal_op(struct expr *, char, double, double);


#endif
