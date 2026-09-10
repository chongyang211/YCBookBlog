/* 运行期动态值（§08）：tagged union——动态类型语言的多态容器 */
#ifndef MYCC_VALUE_H
#define MYCC_VALUE_H

typedef enum {
    VAL_NIL,
    VAL_BOOL,
    VAL_NUM,
    VAL_STR,
    VAL_FN,         /* 函数表下标 */
} ValueKind;

typedef struct {
    ValueKind kind;
    union {
        int    boolean;
        double num;
        char  *str;        /* 堆字符串——"压栈即转交"所有权 */
        int    fn_index;
    } as;
} Value;

static inline Value v_nil(void)     { Value v = {VAL_NIL,  {0}}; return v; }
static inline Value v_bool(int b)   { Value v = {VAL_BOOL, {.boolean = b ? 1 : 0}}; return v; }
static inline Value v_num(double d) { Value v; v.kind = VAL_NUM; v.as.num = d; return v; }
static inline Value v_str(char *s)  { Value v; v.kind = VAL_STR; v.as.str = s; return v; }
static inline Value v_fn(int idx)   { Value v; v.kind = VAL_FN;  v.as.fn_index = idx; return v; }

static inline int is_truthy(const Value *v) {
    switch (v->kind) {
    case VAL_NIL:  return 0;
    case VAL_BOOL: return v->as.boolean;
    case VAL_NUM:  return v->as.num != 0.0;
    case VAL_STR:  return v->as.str && v->as.str[0] != '\0';
    case VAL_FN:   return 1;
    }
    return 0;
}

int  value_equals(const Value *a, const Value *b);
void value_print (const Value *v);
void value_free  (Value *v);

#endif
