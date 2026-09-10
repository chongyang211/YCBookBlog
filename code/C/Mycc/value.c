#include "value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int value_equals(const Value *a, const Value *b) {
    if (a->kind != b->kind) return 0;
    switch (a->kind) {
    case VAL_NIL:  return 1;
    case VAL_BOOL: return a->as.boolean == b->as.boolean;
    case VAL_NUM:  return a->as.num     == b->as.num;
    case VAL_STR:  return strcmp(a->as.str, b->as.str) == 0;
    case VAL_FN:   return a->as.fn_index == b->as.fn_index;
    }
    return 0;
}

void value_print(const Value *v) {
    switch (v->kind) {
    case VAL_NIL:  fputs("nil", stdout); break;
    case VAL_BOOL: fputs(v->as.boolean ? "true" : "false", stdout); break;
    case VAL_NUM:  printf("%g", v->as.num); break;
    case VAL_STR:  fputs(v->as.str, stdout); break;
    case VAL_FN:   printf("<fn#%d>", v->as.fn_index); break;
    }
}

void value_free(Value *v) {
    if (v->kind == VAL_STR && v->as.str) {
        free(v->as.str);
        v->as.str = NULL;
    }
}
