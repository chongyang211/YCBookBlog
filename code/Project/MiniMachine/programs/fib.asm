; ============================================================
; fib.asm — 斐波那契第 10 项, MiniMachine 毕业演示程序
; 结果留在 R2: fib(10) = 55
; 用法: ./build/masm programs/fib.asm && ./build/mmach programs/fib.bin
; ============================================================

    MOV   R1, #0        ; a = fib(0)
    MOV   R2, #1        ; b = fib(1)
    MOV   R3, #9        ; 迭代 9 次后 R2 = fib(10)

loop:
    ADD   R4, R1, R2    ; t = a + b
    ADD   R1, R2, #0    ; a = b
    ADD   R2, R4, #0    ; b = t
    SUB   R3, R3, #1    ; count--
    JZ    R3, done      ; count == 0 → 结束
    JMP   loop

done:
    HALT
