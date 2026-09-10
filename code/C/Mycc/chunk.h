/* Chunk（§07）：字节码容器 = code + 常量池 + 行号表
 * 类比 x86：code=.text / constants=.rodata / lines=DWARF / name=符号表 */
#ifndef MYCC_CHUNK_H
#define MYCC_CHUNK_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "opcode.h"

typedef enum { CV_NUM, CV_BOOL, CV_STR } ConstKind;

typedef struct {
    ConstKind kind;
    union {
        double num;
        int    boolean;
        char  *str;          /* 堆分配，chunk_free 释放 */
    } as;
} Constant;

typedef struct {
    uint8_t  *code;
    size_t    code_count;
    size_t    code_cap;

    int      *lines;         /* 与 code 一一对应的行号 */
    size_t    lines_cap;

    Constant *constants;     /* 常量池（去重存储）*/
    size_t    const_count;
    size_t    const_cap;

    char      name[64];      /* 调试：函数名 / "<top>" */
} Chunk;

void   chunk_init(Chunk *c, const char *name);
void   chunk_free(Chunk *c);

void   chunk_emit(Chunk *c, uint8_t byte, int line);
void   chunk_emit_op(Chunk *c, OpCode op, int line);
void   chunk_emit_u16(Chunk *c, OpCode op, uint16_t operand, int line);

uint16_t chunk_add_const_num (Chunk *c, double v);
uint16_t chunk_add_const_bool(Chunk *c, int v);
uint16_t chunk_add_const_str (Chunk *c, const char *s);

/* 跳转回填三连（§7.1 核心技巧："留个洞、回头补"）*/
size_t chunk_emit_jump(Chunk *c, OpCode op, int line);
void   chunk_patch_jump(Chunk *c, size_t idx);
void   chunk_emit_loop (Chunk *c, size_t target, int line);

void   chunk_disassemble(const Chunk *c, FILE *out);

#endif
