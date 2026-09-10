/* VM（§08）：fetch-decode-execute 主循环 + CallFrame 调用栈
 * mycc 的求值栈/调用栈在 VM 结构体里——fib 递归再深也不爆 C 栈 */
#include "vm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"

void vm_init(VM *vm) { memset(vm, 0, sizeof(*vm)); }

void vm_free(VM *vm) {
    for (int i = 0; i < vm->sp; ++i) value_free(&vm->stack[i]);
    for (int i = 0; i < vm->global_count; ++i) value_free(&vm->globals[i].value);
    memset(vm, 0, sizeof(*vm));
}

void vm_load(VM *vm, const CompiledFn *fns, int fn_count) {
    vm->fns = fns;
    vm->fn_count = fn_count;
}

static void push(VM *vm, Value v) {
    if (vm->sp >= MAX_VM_STACK)
        mycc_error(ERR_RUNTIME, 0, NULL, "stack overflow");
    vm->stack[vm->sp++] = v;
}
static Value pop(VM *vm)  { return vm->stack[--vm->sp]; }
static Value peek(VM *vm, int depth) { return vm->stack[vm->sp - 1 - depth]; }

static char *str_dup_heap(const char *s) {
    char *dup = (char *)malloc(strlen(s) + 1);
    if (!dup) mycc_error(ERR_RUNTIME, 0, NULL, "out of memory");
    strcpy(dup, s);
    return dup;
}

/* ---- 字节码解码 ---- */
static uint8_t read_byte(VM *vm) {
    CallFrame *f = &vm->frames[vm->frame_count - 1];
    return f->chunk->code[f->ip++];
}
static uint16_t read_u16(VM *vm) {
    uint16_t lo = read_byte(vm);
    uint16_t hi = read_byte(vm);
    return (uint16_t)(lo | (hi << 8));
}
static const Constant *read_const(VM *vm) {
    CallFrame *f = &vm->frames[vm->frame_count - 1];
    return &f->chunk->constants[read_u16(vm)];
}

/* ---- 错误：带调用栈回溯 ---- */
static void runtime_error(VM *vm, const char *msg, int line) {
    char trace[1024];
    int n = snprintf(trace, sizeof trace, "%s", msg);
    for (int i = vm->frame_count - 1; i >= 0 && n < (int)sizeof(trace) - 64; --i) {
        n += snprintf(trace + n, sizeof(trace) - n, "\n  in %s", vm->frames[i].name);
    }
    mycc_error(ERR_RUNTIME, line, NULL, "%s", trace);
    (void)vm;
}

/* ---- 全局变量与函数表 ---- */
static Global *find_global(VM *vm, const char *name) {
    for (int i = 0; i < vm->global_count; ++i) {
        if (strcmp(vm->globals[i].name, name) == 0) return &vm->globals[i];
    }
    return NULL;
}

static int find_fn_index(VM *vm, const char *name) {
    for (int i = 0; i < vm->fn_count; ++i) {
        if (strcmp(vm->fns[i].name, name) == 0) return i;
    }
    return -1;
}

/* ---- 二元运算（含字符串拼接）---- */
static void binary_arith(VM *vm, OpCode op, int line) {
    Value b = pop(vm), a = pop(vm);
    if (op == OP_ADD && a.kind == VAL_STR && b.kind == VAL_STR) {
        /* 字符串拼接：教程 §9.5 留的练习，配套工程已实现 */
        size_t la = strlen(a.as.str), lb = strlen(b.as.str);
        char *s = (char *)malloc(la + lb + 1);
        if (!s) mycc_error(ERR_RUNTIME, line, NULL, "out of memory");
        memcpy(s, a.as.str, la);
        memcpy(s + la, b.as.str, lb + 1);
        value_free(&a); value_free(&b);
        push(vm, v_str(s));
        return;
    }
    if (a.kind != VAL_NUM || b.kind != VAL_NUM) {
        value_free(&a); value_free(&b);
        runtime_error(vm, "operands must be numbers", line);
        return;
    }
    double x = a.as.num, y = b.as.num;
    switch (op) {
    case OP_ADD: push(vm, v_num(x + y)); break;
    case OP_SUB: push(vm, v_num(x - y)); break;
    case OP_MUL: push(vm, v_num(x * y)); break;
    case OP_DIV:
        if (y == 0.0) { runtime_error(vm, "divide by zero", line); return; }
        push(vm, v_num(x / y)); break;
    case OP_MOD:
        if (y == 0.0) { runtime_error(vm, "mod by zero", line); return; }
        push(vm, v_num(fmod(x, y))); break;
    default: break;
    }
}

static void binary_cmp(VM *vm, OpCode op, int line) {
    Value b = pop(vm), a = pop(vm);
    if (op == OP_EQ)  { int r = value_equals(&a, &b); value_free(&a); value_free(&b); push(vm, v_bool(r));  return; }
    if (op == OP_NEQ) { int r = value_equals(&a, &b); value_free(&a); value_free(&b); push(vm, v_bool(!r)); return; }
    if (a.kind != VAL_NUM || b.kind != VAL_NUM) {
        value_free(&a); value_free(&b);
        runtime_error(vm, "comparison operands must be numbers", line);
        return;
    }
    double x = a.as.num, y = b.as.num;
    switch (op) {
    case OP_LT: push(vm, v_bool(x <  y)); break;
    case OP_LE: push(vm, v_bool(x <= y)); break;
    case OP_GT: push(vm, v_bool(x >  y)); break;
    case OP_GE: push(vm, v_bool(x >= y)); break;
    default: break;
    }
}

/* ============================================================
 *  主循环：fetch / decode / execute
 * ============================================================ */
void vm_run(VM *vm) {
    if (vm->fn_count == 0) return;

    /* 主栈帧：fns[0] 是 <top> */
    vm->frame_count = 1;
    vm->frames[0].chunk     = &vm->fns[0].chunk;
    vm->frames[0].ip        = 0;
    vm->frames[0].slot_base = 0;
    vm->frames[0].name      = vm->fns[0].name;
    vm->sp                  = 0;

    while (1) {
        CallFrame *fr = &vm->frames[vm->frame_count - 1];
        int line = fr->chunk->lines[fr->ip];
        OpCode op = (OpCode)read_byte(vm);

        switch (op) {
        /* ---- 字面量 ---- */
        case OP_CONST: {
            const Constant *c = read_const(vm);
            switch (c->kind) {
            case CV_NUM:  push(vm, v_num(c->as.num));      break;
            case CV_BOOL: push(vm, v_bool(c->as.boolean)); break;
            case CV_STR:  push(vm, v_str(str_dup_heap(c->as.str))); break;
            }
            break;
        }
        case OP_NIL:   push(vm, v_nil());   break;
        case OP_TRUE:  push(vm, v_bool(1)); break;
        case OP_FALSE: push(vm, v_bool(0)); break;

        /* ---- 算术 / 比较 ---- */
        case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV: case OP_MOD:
            binary_arith(vm, op, line);
            if (vm->had_error) return;
            break;
        case OP_EQ: case OP_NEQ: case OP_LT: case OP_LE: case OP_GT: case OP_GE:
            binary_cmp(vm, op, line);
            if (vm->had_error) return;
            break;

        case OP_NEG: {
            Value a = pop(vm);
            if (a.kind != VAL_NUM) { runtime_error(vm, "unary '-' needs number", line); return; }
            push(vm, v_num(-a.as.num));
            break;
        }
        case OP_NOT: {
            Value a = pop(vm);
            int r = !is_truthy(&a);
            value_free(&a);
            push(vm, v_bool(r));
            break;
        }

        /* ---- 变量 ---- */
        case OP_LOAD_GLOBAL: {
            const Constant *c = read_const(vm);
            const char *name = c->as.str;
            int fn_idx = find_fn_index(vm, name);   /* 优先函数表 */
            if (fn_idx >= 0) { push(vm, v_fn(fn_idx)); break; }
            Global *g = find_global(vm, name);
            if (!g) { runtime_error(vm, "undefined variable", line); return; }
            Value cp = g->value;                    /* 深拷贝字符串入栈 */
            if (cp.kind == VAL_STR) cp.as.str = str_dup_heap(g->value.as.str);
            push(vm, cp);
            break;
        }
        case OP_STORE_GLOBAL: {
            const Constant *c = read_const(vm);
            const char *name = c->as.str;
            Global *g = find_global(vm, name);
            if (!g) {
                if (vm->global_count >= MAX_GLOBALS) {
                    runtime_error(vm, "too many globals", line); return;
                }
                g = &vm->globals[vm->global_count++];
                strncpy(g->name, name, 31); g->name[31] = '\0';
                g->value = v_nil();
            }
            value_free(&g->value);
            Value top = peek(vm, 0);                /* peek 不弹（DUP 已备份）*/
            if (top.kind == VAL_STR) g->value = v_str(str_dup_heap(top.as.str));
            else                     g->value = top;
            break;
        }
        case OP_LOAD_LOCAL: {
            uint16_t slot = read_u16(vm);
            Value cp = vm->stack[fr->slot_base + slot];
            if (cp.kind == VAL_STR) cp.as.str = str_dup_heap(cp.as.str);
            push(vm, cp);
            break;
        }
        case OP_STORE_LOCAL: {
            uint16_t slot = read_u16(vm);
            value_free(&vm->stack[fr->slot_base + slot]);
            Value top = peek(vm, 0);
            if (top.kind == VAL_STR) vm->stack[fr->slot_base + slot] = v_str(str_dup_heap(top.as.str));
            else                     vm->stack[fr->slot_base + slot] = top;
            break;
        }

        /* ---- 控制流 ---- */
        case OP_JUMP: {
            uint16_t off = read_u16(vm);
            fr->ip += off;
            break;
        }
        case OP_JUMP_IF_FALSE: {
            uint16_t off = read_u16(vm);
            Value v = peek(vm, 0);
            if (!is_truthy(&v)) fr->ip += off;
            break;
        }
        case OP_JUMP_IF_TRUE: {
            uint16_t off = read_u16(vm);
            Value v = peek(vm, 0);
            if (is_truthy(&v)) fr->ip += off;
            break;
        }
        case OP_POP_JUMP_IF_FALSE: {
            uint16_t off = read_u16(vm);
            Value v = pop(vm);
            int truthy = is_truthy(&v);
            value_free(&v);
            if (!truthy) fr->ip += off;
            break;
        }
        case OP_LOOP: {
            uint16_t off = read_u16(vm);
            fr->ip -= off;
            break;
        }

        /* ---- 函数调用 ----
         * 栈布局：[..., callee(v_fn), arg0, ..., argN]
         * slot_base = sp - argc 指向 arg0；RETURN 清到 slot_base-1 */
        case OP_CALL: {
            uint8_t argc = read_byte(vm);
            Value callee = vm->stack[vm->sp - argc - 1];
            if (callee.kind != VAL_FN) {
                runtime_error(vm, "can only call functions", line); return;
            }
            int idx = callee.as.fn_index;
            if (idx < 0 || idx >= vm->fn_count) {
                runtime_error(vm, "invalid function index", line); return;
            }
            if (vm->frame_count >= MAX_VM_FRAMES) {
                runtime_error(vm, "stack overflow (recursion too deep)", line); return;
            }
            CallFrame *nf = &vm->frames[vm->frame_count++];
            nf->chunk     = &vm->fns[idx].chunk;
            nf->ip        = 0;
            nf->slot_base = vm->sp - argc;
            nf->name      = vm->fns[idx].name;
            break;
        }

        case OP_RETURN: {
            Value rv = pop(vm);
            CallFrame done = vm->frames[--vm->frame_count];
            if (vm->frame_count == 0) {
                /* <top> 也通过 RETURN 退出 */
                value_free(&rv);
                return;
            }
            /* 弹掉被调函数的所有 locals + callee 自身（含字符串释放）*/
            while (vm->sp > (int)done.slot_base - 1) {
                Value t = pop(vm);
                value_free(&t);
            }
            push(vm, rv);
            break;
        }

        /* ---- I/O / 控制 ---- */
        case OP_PRINT: {
            Value v = pop(vm);
            value_print(&v);
            fputc('\n', stdout);
            value_free(&v);
            break;
        }
        case OP_POP: { Value v = pop(vm); value_free(&v); break; }
        case OP_DUP: {
            Value top = peek(vm, 0);
            if (top.kind == VAL_STR) push(vm, v_str(str_dup_heap(top.as.str)));
            else                     push(vm, top);
            break;
        }
        case OP_HALT: return;

        default:
            runtime_error(vm, "unknown opcode (jump misaligned?)", line);
            return;
        }
    }
}
