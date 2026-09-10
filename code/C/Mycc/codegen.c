/* Codegen（§07）：AST → 字节码。核心技巧：跳转回填（"留个洞、回头补"）
 * 教程原代码两处栈不平衡 bug 已修复（见 gen_call / gen_var_decl 注释）*/
#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void codegen_init(Codegen *cg) {
    memset(cg, 0, sizeof(*cg));
    strcpy(cg->fns[0].name, "<top>");
    chunk_init(&cg->fns[0].chunk, "<top>");
    cg->fn_count = 1;
    cg->cur = &cg->fns[0].chunk;
}

void codegen_free(Codegen *cg) {
    for (int i = 0; i < cg->fn_count; ++i) chunk_free(&cg->fns[i].chunk);
}

static void gen(Codegen *cg, AstNode *n);

/* ---- 局部变量解析（仅在函数体内使用）---- */
static int resolve_local(Codegen *cg, const char *name) {
    for (int i = cg->local_count - 1; i >= 0; --i) {
        if (strcmp(cg->locals[i].name, name) == 0) return i;
    }
    return -1;
}

static void add_local(Codegen *cg, const char *name) {
    if (cg->local_count >= MAX_LOCALS) {
        fprintf(stderr, "[Codegen] too many locals\n"); exit(1);
    }
    LocalVar *v = &cg->locals[cg->local_count++];
    strncpy(v->name, name, 31); v->name[31] = '\0';
    v->depth = cg->scope_depth;
}

/* ---- 表达式 ---- */
static void gen_num_lit(Codegen *cg, AstNode *n) {
    uint16_t k = chunk_add_const_num(cg->cur, n->as.num_lit.value);
    chunk_emit_u16(cg->cur, OP_CONST, k, n->line);
}

static void gen_bool_lit(Codegen *cg, AstNode *n) {
    chunk_emit_op(cg->cur, n->as.bool_lit.value ? OP_TRUE : OP_FALSE, n->line);
}

static void gen_str_lit(Codegen *cg, AstNode *n) {
    uint16_t k = chunk_add_const_str(cg->cur, n->as.str_lit.value);
    chunk_emit_u16(cg->cur, OP_CONST, k, n->line);
}

static void gen_var_ref(Codegen *cg, AstNode *n) {
    if (cg->in_function) {
        int slot = resolve_local(cg, n->as.var_ref.name);
        if (slot >= 0) {
            chunk_emit_u16(cg->cur, OP_LOAD_LOCAL, (uint16_t)slot, n->line);
            return;
        }
    }
    uint16_t k = chunk_add_const_str(cg->cur, n->as.var_ref.name);
    chunk_emit_u16(cg->cur, OP_LOAD_GLOBAL, k, n->line);
}

static void gen_assign(Codegen *cg, AstNode *n) {
    gen(cg, n->as.assign.value);
    chunk_emit_op(cg->cur, OP_DUP, n->line);     /* 赋值表达式的值留在栈上 */
    if (cg->in_function) {
        int slot = resolve_local(cg, n->as.assign.name);
        if (slot >= 0) {
            chunk_emit_u16(cg->cur, OP_STORE_LOCAL, (uint16_t)slot, n->line);
            return;
        }
    }
    uint16_t k = chunk_add_const_str(cg->cur, n->as.assign.name);
    chunk_emit_u16(cg->cur, OP_STORE_GLOBAL, k, n->line);
}

static void gen_bin_op(Codegen *cg, AstNode *n) {
    TokKind op = n->as.bin_op.op;
    /* ⭐ 短路 &&：左为假就跳过右，结果是栈顶的假值 */
    if (op == TK_ANDAND) {
        gen(cg, n->as.bin_op.lhs);
        size_t end_jmp = chunk_emit_jump(cg->cur, OP_JUMP_IF_FALSE, n->line);
        chunk_emit_op (cg->cur, OP_POP, n->line);  /* 弹掉左值 */
        gen(cg, n->as.bin_op.rhs);
        chunk_patch_jump(cg->cur, end_jmp);
        return;
    }
    /* ⭐ 短路 || */
    if (op == TK_OROR) {
        gen(cg, n->as.bin_op.lhs);
        size_t end_jmp = chunk_emit_jump(cg->cur, OP_JUMP_IF_TRUE, n->line);
        chunk_emit_op (cg->cur, OP_POP, n->line);
        gen(cg, n->as.bin_op.rhs);
        chunk_patch_jump(cg->cur, end_jmp);
        return;
    }
    gen(cg, n->as.bin_op.lhs);
    gen(cg, n->as.bin_op.rhs);
    OpCode oc;
    switch (op) {
    case TK_PLUS:    oc = OP_ADD; break;
    case TK_MINUS:   oc = OP_SUB; break;
    case TK_STAR:    oc = OP_MUL; break;
    case TK_SLASH:   oc = OP_DIV; break;
    case TK_PERCENT: oc = OP_MOD; break;
    case TK_EQ:      oc = OP_EQ;  break;
    case TK_NE:      oc = OP_NEQ; break;
    case TK_LT:      oc = OP_LT;  break;
    case TK_LE:      oc = OP_LE;  break;
    case TK_GT:      oc = OP_GT;  break;
    case TK_GE:      oc = OP_GE;  break;
    default:
        fprintf(stderr, "[Codegen] unsupported binary op\n"); exit(1);
    }
    chunk_emit_op(cg->cur, oc, n->line);
}

static void gen_unary(Codegen *cg, AstNode *n) {
    gen(cg, n->as.unary_op.operand);
    chunk_emit_op(cg->cur,
                  n->as.unary_op.op == TK_MINUS ? OP_NEG : OP_NOT,
                  n->line);
}

static void gen_call(Codegen *cg, AstNode *n) {
    /* ⭐ 修复教程 bug：压 OP_LOAD_GLOBAL（VM 查函数表转成 v_fn 入栈），
     * 原 OP_CONST 只压字符串，OP_CALL 期望 VAL_FN 会报 can only call */
    uint16_t k = chunk_add_const_str(cg->cur, n->as.call_expr.callee);
    chunk_emit_u16(cg->cur, OP_LOAD_GLOBAL, k, n->line);
    for (int i = 0; i < n->as.call_expr.n_args; ++i) gen(cg, n->as.call_expr.args[i]);
    chunk_emit_op(cg->cur, OP_CALL, n->line);
    chunk_emit   (cg->cur, (uint8_t)n->as.call_expr.n_args, n->line);
}

/* ---- 语句 ---- */
static void gen_print(Codegen *cg, AstNode *n) {
    gen(cg, n->as.print_stmt.expr);
    chunk_emit_op(cg->cur, OP_PRINT, n->line);
}

static void gen_expr_stmt(Codegen *cg, AstNode *n) {
    gen(cg, n->as.expr_stmt.expr);
    chunk_emit_op(cg->cur, OP_POP, n->line);
}

static void gen_var_decl(Codegen *cg, AstNode *n) {
    if (n->as.var_decl.init) gen(cg, n->as.var_decl.init);
    else                     chunk_emit_op(cg->cur, OP_NIL, n->line);

    if (cg->in_function) {
        add_local(cg, n->as.var_decl.name);
        /* 局部变量值留在栈顶 = locals[slot]（VM 的 slot_base 定位）*/
    } else {
        uint16_t k = chunk_add_const_str(cg->cur, n->as.var_decl.name);
        chunk_emit_u16(cg->cur, OP_STORE_GLOBAL, k, n->line);
        /* ⭐ 修复教程 bug：STORE_GLOBAL 是 peek 语义（assign 配 DUP 用），
         * 这里 init 值已入全局表，必须补弹保持栈平衡 */
        chunk_emit_op(cg->cur, OP_POP, n->line);
    }
}

/* ⭐ if/while 的回填核心 */
static void gen_if(Codegen *cg, AstNode *n) {
    gen(cg, n->as.if_stmt.cond);
    size_t else_jmp = chunk_emit_jump(cg->cur, OP_POP_JUMP_IF_FALSE, n->line);
    gen(cg, n->as.if_stmt.then_branch);
    size_t end_jmp  = chunk_emit_jump(cg->cur, OP_JUMP, n->line);
    chunk_patch_jump(cg->cur, else_jmp);         /* else 入口回填 */
    if (n->as.if_stmt.else_branch) gen(cg, n->as.if_stmt.else_branch);
    chunk_patch_jump(cg->cur, end_jmp);          /* 结尾回填 */
}

static void gen_while(Codegen *cg, AstNode *n) {
    size_t loop_start = cg->cur->code_count;
    gen(cg, n->as.while_stmt.cond);
    size_t exit_jmp = chunk_emit_jump(cg->cur, OP_POP_JUMP_IF_FALSE, n->line);
    gen(cg, n->as.while_stmt.body);
    chunk_emit_loop (cg->cur, loop_start, n->line);
    chunk_patch_jump(cg->cur, exit_jmp);
}

static void gen_block(Codegen *cg, AstNode *n) {
    cg->scope_depth++;
    int saved = cg->local_count;
    for (int i = 0; i < n->as.block.n_stmts; ++i) gen(cg, n->as.block.stmts[i]);
    cg->scope_depth--;
    /* 弹掉本块定义的局部变量（栈平衡）*/
    while (cg->local_count > saved) {
        chunk_emit_op(cg->cur, OP_POP, n->line);
        cg->local_count--;
    }
}

static void gen_fn_decl(Codegen *cg, AstNode *n) {
    if (cg->fn_count >= MAX_FNS) {
        fprintf(stderr, "[Codegen] too many functions\n"); exit(1);
    }
    CompiledFn *fn = &cg->fns[cg->fn_count++];
    strncpy(fn->name, n->as.fn_decl.name, 31);
    fn->name[31] = '\0';
    fn->param_count = n->as.fn_decl.n_params;
    chunk_init(&fn->chunk, n->as.fn_decl.name);

    /* 切换上下文到函数体 */
    Chunk *saved_cur    = cg->cur;
    int   saved_local   = cg->local_count;
    int   saved_in_fn   = cg->in_function;
    int   saved_depth   = cg->scope_depth;

    cg->cur         = &fn->chunk;
    cg->local_count = 0;
    cg->in_function = 1;
    cg->scope_depth = 0;

    for (int i = 0; i < n->as.fn_decl.n_params; ++i)   /* 参数 = locals[0..n-1] */
        add_local(cg, n->as.fn_decl.params[i]);
    gen(cg, n->as.fn_decl.body);
    chunk_emit_op(cg->cur, OP_NIL, n->line);     /* 默认返回 nil */
    chunk_emit_op(cg->cur, OP_RETURN, n->line);

    cg->cur         = saved_cur;
    cg->local_count = saved_local;
    cg->in_function = saved_in_fn;
    cg->scope_depth = saved_depth;
}

static void gen_return(Codegen *cg, AstNode *n) {
    if (n->as.return_stmt.value) gen(cg, n->as.return_stmt.value);
    else                         chunk_emit_op(cg->cur, OP_NIL, n->line);
    chunk_emit_op(cg->cur, OP_RETURN, n->line);
}

/* ---- 主分派 ---- */
static void gen(Codegen *cg, AstNode *n) {
    if (!n) return;
    switch (n->kind) {
    case NK_NUM_LIT:     gen_num_lit(cg, n);  break;
    case NK_BOOL_LIT:    gen_bool_lit(cg, n); break;
    case NK_STR_LIT:     gen_str_lit(cg, n);  break;
    case NK_VAR_REF:     gen_var_ref(cg, n);  break;
    case NK_BIN_OP:      gen_bin_op(cg, n);   break;
    case NK_UNARY_OP:    gen_unary(cg, n);    break;
    case NK_ASSIGN:      gen_assign(cg, n);   break;
    case NK_CALL_EXPR:   gen_call(cg, n);     break;
    case NK_PRINT_STMT:  gen_print(cg, n);    break;
    case NK_EXPR_STMT:   gen_expr_stmt(cg, n);break;
    case NK_VAR_DECL:    gen_var_decl(cg, n); break;
    case NK_IF_STMT:     gen_if(cg, n);       break;
    case NK_WHILE_STMT:  gen_while(cg, n);    break;
    case NK_BLOCK:       gen_block(cg, n);    break;
    case NK_FN_DECL:     gen_fn_decl(cg, n);  break;
    case NK_RETURN_STMT: gen_return(cg, n);   break;
    case NK_PROGRAM:
        for (int i = 0; i < n->as.program.n_decls; ++i)
            gen(cg, n->as.program.decls[i]);
        break;
    }
}

void codegen_program(Codegen *cg, AstNode *prog) {
    gen(cg, prog);
    chunk_emit_op(cg->cur, OP_HALT, 0);
}
