/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2019, Andrea Giacomo Baldan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define HASHTABLE_OK   0
#define HASHTABLE_ERR  1
#define HASHTABLE_OOM  2
#define HASHTABLE_FULL 3


/* We need to keep keys and values */
struct ht_entry {
    const char *key;
    void *val;
    bool taken;
};


typedef int ht_unary_fun(struct ht_entry *);


typedef int ht_binary_fun(struct ht_entry *, void *);


/*
 * An HashTable has some maximum size and current size, as well as the data to
 * hold.
 */
typedef struct {
    size_t table_size;
    size_t size;
    ht_unary_fun *destructor;
    struct ht_entry *entries;
} HashTable;


/*
 * HashTable API to create a new `HashTable`, it require a function pointer to
 * define a custom destructor, which can be NULL in case of bytes stream or
 * simple pointers as values
 */
HashTable *hashtable_create(ht_unary_fun *);

/* Init hasthable data */
void hashtable_init(HashTable *, ht_unary_fun *);

/* Destroy the hashtable by calling functor `destructor` on every
 * `struct ht_entry`, thus it needs to have a defined destructor function for
 * each different data-type inserted. In case of a NULL destructor, it' ll call
 * normal free.
 */
void hashtable_release(HashTable *);

/* Return hashtable size */
size_t hashtable_size(const HashTable *);

/*
 * Insert a new key-value pair into the hashtable, accept a const char * as
 * key and a void * for value
 */
int hashtable_put(HashTable *, const char *, void *);

/* Retrieve a value from the hashtable, accept a const char * as key. */
void *hashtable_get(HashTable *, const char *);

/* Remove a key-value pair from the hashtable, accept a const char * as key. */
int hashtable_del(HashTable *, const char *);

/*
 * Iterate through all key-value pairs in the hashtable, accept a functor as
 * parameter to apply function to each pair
 */
int hashtable_map(HashTable *, ht_unary_fun *);

/*
 * Iterate through all key-value pairs in the hashtable, accept a functor as
 * parameter to apply function to each pair with an additional parameter
 */
int hashtable_map2(HashTable *, ht_binary_fun *, void *);


#endif
