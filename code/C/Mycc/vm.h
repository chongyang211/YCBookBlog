#ifndef MYCC_VM_H
#define MYCC_VM_H

#include "chunk.h"
#include "codegen.h"
#include "value.h"

#define MAX_VM_STACK   1024
#define MAX_VM_FRAMES  64
#define MAX_GLOBALS    256

typedef struct {
    const Chunk *chunk;       /* 当前执行的字节码 */
    size_t       ip;          /* 指令指针 */
    size_t       slot_base;   /* 该函数局部变量在 stack 中的起始位置 */
    const char  *name;        /* 调试用 */
} CallFrame;

typedef struct {
    char  name[32];
    Value value;
} Global;

typedef struct {
    const CompiledFn *fns;    /* 编译期产物（外部注入）*/
    int               fn_count;

    Value      stack[MAX_VM_STACK];
    int        sp;

    CallFrame  frames[MAX_VM_FRAMES];
    int        frame_count;

    Global     globals[MAX_GLOBALS];
    int        global_count;

    int        had_error;
} VM;

void vm_init(VM *vm);
void vm_free(VM *vm);
void vm_load(VM *vm, const CompiledFn *fns, int fn_count);
void vm_run(VM *vm);

#endif
