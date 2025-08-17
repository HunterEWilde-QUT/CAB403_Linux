#include <stdio.h>
#include <stdlib.h>
#include "dbl_vector.h"

void dv_init( dbl_vector_t* vec ) {
    vec->capacity = DV_INITIAL_CAPACITY;
    vec->size = 0;
    vec->data = malloc(DV_INITIAL_CAPACITY * sizeof(double));
    return;
}

void dv_ensure_capacity( dbl_vector_t* vec, size_t new_size ) {
    if(new_size > vec->capacity) {
        vec->capacity = new_size;
        vec->data = realloc(vec->data, new_size * sizeof(double));
    }
    return;
}

void dv_destroy( dbl_vector_t* vec ) {
    vec->capacity = 0;
    vec->size = 0;
    free(vec->data);
    vec->data = NULL;
    return;
}

void dv_copy( dbl_vector_t* vec, dbl_vector_t* dest ) {
    dest->size = vec->size;
    dv_ensure_capacity(dest, vec->size);
    for(int i = 0; i < vec->size; i++) {
        dest->data[i] = vec->data[i];
    }
    return;
}

void dv_clear( dbl_vector_t* vec ) {
    vec->size = 0;
    return;
}

void dv_push( dbl_vector_t* vec, double new_item ) {
    size_t old_size = vec->size;
    vec->size = old_size + 1;
    dv_ensure_capacity(vec, vec->size);
    vec->data[old_size] = new_item;
    return;
}

void dv_pop( dbl_vector_t* vec ) {
    size_t old_size = vec->size;
    if(old_size > 0) {
        vec->size = old_size - 1;
    }
    return;
}

double dv_last( dbl_vector_t* vec ) {
    double result = NAN;
    if(vec->size > 0) {
        result = vec->data[vec->size - 1];
    }
    return result;
}

void dv_insert_at( dbl_vector_t* vec, size_t pos, double new_item ) {
    if(pos > vec->size) dv_push(vec, new_item); // append item if position exceeds vector size
    else {
        size_t old_size = vec->size;
        vec->size = old_size + 1;
        dv_ensure_capacity(vec, old_size + 1);
        // Working backwards from the end of the new vector, move each datum to the next position
        for(int i = vec->size; i > pos; i--) {
            vec->data[i] = vec->data[i - 1];
        }
        vec->data[pos] = new_item; // Add the new datum into the space created
    }
    return;
}

void dv_remove_at( dbl_vector_t* vec, size_t pos ) {
    size_t old_size = vec->size;
    if(pos < old_size) {
        vec->size = old_size - 1;
        // Starting with the target datum, overwrite each datum with the next datum
        for(int i = pos; i < vec->size; i++) {
            vec->data[i] = vec->data[i + 1];
        }
    }
    return;
}

void dv_foreach( dbl_vector_t* vec, void (*callback)(double, void*), void* info ) {
    for(int i = 0; i < vec->size; i++) {
        callback(vec->data[i], info);
    }
    return;
}
