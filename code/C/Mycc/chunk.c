#include "chunk.h"

#include <stdlib.h>
#include <string.h>

void chunk_init(Chunk *c, const char *name) {
    memset(c, 0, sizeof(*c));
    strncpy(c->name, name ? name : "<chunk>", sizeof c->name - 1);
}

void chunk_free(Chunk *c) {
    free(c->code);
    free(c->lines);
    for (size_t i = 0; i < c->const_count; ++i) {
        if (c->constants[i].kind == CV_STR) free(c->constants[i].as.str);
    }
    free(c->constants);
    memset(c, 0, sizeof(*c));
}

static void grow_code(Chunk *c) {
    size_t newcap = c->code_cap < 64 ? 64 : c->code_cap * 2;
    uint8_t *nc = (uint8_t *)realloc(c->code, newcap);
    int *nl = (int *)realloc(c->lines, newcap * sizeof(int));
    if (!nc || !nl) { perror("realloc"); exit(1); }
    c->code = nc;
    c->lines = nl;
    c->code_cap = c->lines_cap = newcap;
}

void chunk_emit(Chunk *c, uint8_t byte, int line) {
    if (c->code_count >= c->code_cap) grow_code(c);
    c->code[c->code_count]  = byte;
    c->lines[c->code_count] = line;
    c->code_count++;
}

void chunk_emit_op(Chunk *c, OpCode op, int line) { chunk_emit(c, (uint8_t)op, line); }

void chunk_emit_u16(Chunk *c, OpCode op, uint16_t operand, int line) {
    chunk_emit_op(c, op, line);
    chunk_emit(c,  operand & 0xFF, line);          /* 小端：低字节在前 */
    chunk_emit(c, (operand >> 8) & 0xFF, line);
}

/* 常量池——去重存储 */
static uint16_t add_const(Chunk *c, Constant cv) {
    for (size_t i = 0; i < c->const_count; ++i) {
        Constant *e = &c->constants[i];
        if (e->kind != cv.kind) continue;
        switch (cv.kind) {
        case CV_NUM:  if (e->as.num == cv.as.num) return (uint16_t)i; break;
        case CV_BOOL: if (e->as.boolean == cv.as.boolean) return (uint16_t)i; break;
        case CV_STR:
            if (strcmp(e->as.str, cv.as.str) == 0) {
                free(cv.as.str);                   /* 释放重复字符串 */
                return (uint16_t)i;
            }
            break;
        }
    }
    if (c->const_count >= c->const_cap) {
        size_t nc = c->const_cap < 8 ? 8 : c->const_cap * 2;
        Constant *np = (Constant *)realloc(c->constants, nc * sizeof(Constant));
        if (!np) { perror("realloc"); exit(1); }
        c->constants = np;
        c->const_cap = nc;
    }
    if (c->const_count >= 65535) {
        fprintf(stderr, "too many constants in one chunk\n"); exit(1);
    }
    c->constants[c->const_count] = cv;
    return (uint16_t)c->const_count++;
}

uint16_t chunk_add_const_num(Chunk *c, double v) {
    return add_const(c, (Constant){CV_NUM, {.num = v}});
}
uint16_t chunk_add_const_bool(Chunk *c, int v) {
    return add_const(c, (Constant){CV_BOOL, {.boolean = v}});
}
uint16_t chunk_add_const_str(Chunk *c, const char *s) {
    char *dup = (char *)malloc(strlen(s) + 1);
    if (!dup) { perror("malloc"); exit(1); }
    strcpy(dup, s);
    Constant cv = { CV_STR, {.str = dup} };
    return add_const(c, cv);
}

/* ----- 跳转回填三连 -----
 * emit_jump：写指令 + 0xFFFF 占位，返回占位低字节下标
 * patch_jump：把占位改成"当前位置 - 占位 - 2"的真实偏移
 * ⭐ -2 不能漏：跳转基准是"操作数之后那条指令"（§8.4 造 BUG 现场）*/
size_t chunk_emit_jump(Chunk *c, OpCode op, int line) {
    chunk_emit_op(c, op,   line);
    chunk_emit   (c, 0xFF, line);
    chunk_emit   (c, 0xFF, line);
    return c->code_count - 2;
}

void chunk_patch_jump(Chunk *c, size_t idx) {
    size_t jump = c->code_count - idx - 2;         /* ⭐ */
    if (jump > 0xFFFF) {
        fprintf(stderr, "jump distance too large (>64KB)\n"); exit(1);
    }
    c->code[idx]     =  jump       & 0xFF;
    c->code[idx + 1] = (jump >> 8) & 0xFF;
}

void chunk_emit_loop(Chunk *c, size_t target, int line) {
    chunk_emit_op(c, OP_LOOP, line);
    size_t offset = c->code_count + 2 - target;
    if (offset > 0xFFFF) {
        fprintf(stderr, "loop distance too large\n"); exit(1);
    }
    chunk_emit(c,  offset       & 0xFF, line);
    chunk_emit(c, (offset >> 8) & 0xFF, line);
}

/* ----- 反汇编 ----- */
static size_t dis_simple(FILE *out, const char *name, size_t off) {
    fprintf(out, "%s\n", name);
    return off + 1;
}

static size_t dis_u16(FILE *out, const char *name, const Chunk *c, size_t off) {
    uint16_t arg = (uint16_t)(c->code[off + 1] | (c->code[off + 2] << 8));
    fprintf(out, "%-18s %4u", name, arg);
    if (strcmp(name, "CONST") == 0 ||
        strcmp(name, "LOAD_GLOBAL") == 0 ||
        strcmp(name, "STORE_GLOBAL") == 0) {
        Constant *k = &c->constants[arg];
        fprintf(out, "  ; ");
        switch (k->kind) {
        case CV_NUM:  fprintf(out, "%g", k->as.num); break;
        case CV_BOOL: fprintf(out, k->as.boolean ? "true" : "false"); break;
        case CV_STR:  fprintf(out, "\"%s\"", k->as.str); break;
        }
    }
    fputc('\n', out);
    return off + 3;
}

void chunk_disassemble(const Chunk *c, FILE *out) {
    fprintf(out, "== %s ==\n", c->name);
    for (size_t off = 0; off < c->code_count; ) {
        fprintf(out, "%04zu  ", off);
        if (off > 0 && c->lines[off] == c->lines[off - 1]) fprintf(out, "   |  ");
        else                                                fprintf(out, "%4d  ", c->lines[off]);

        OpCode op = (OpCode)c->code[off];
        switch (op) {
        /* 1 字节指令 */
        case OP_NIL: case OP_TRUE: case OP_FALSE:
        case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV: case OP_MOD: case OP_NEG:
        case OP_EQ: case OP_NEQ: case OP_LT: case OP_LE: case OP_GT: case OP_GE:
        case OP_NOT: case OP_AND: case OP_OR:
        case OP_RETURN: case OP_PRINT: case OP_POP: case OP_DUP: case OP_HALT:
            off = dis_simple(out, opcode_name(op), off);
            break;
        /* 1+2 字节指令 */
        case OP_CONST: case OP_LOAD_GLOBAL: case OP_STORE_GLOBAL:
        case OP_LOAD_LOCAL: case OP_STORE_LOCAL:
        case OP_JUMP: case OP_JUMP_IF_FALSE: case OP_JUMP_IF_TRUE:
        case OP_POP_JUMP_IF_FALSE: case OP_LOOP:
            off = dis_u16(out, opcode_name(op), c, off);
            break;
        case OP_CALL: {                /* CALL 有 1 字节操作数 */
            uint8_t n = c->code[off + 1];
            fprintf(out, "%-18s %4u\n", "CALL", n);
            off += 2;
            break;
        }
        }
    }
}
