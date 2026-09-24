; 计算斐波那契前 10 项之和, 存到 R7
; R0 = 计数器 (i)
; R1 = fib(i-1) = 0
; R2 = fib(i)   = 1
; R3 = 累加和
; R7 = 循环上限 10
start:
    MOV   R1, #0
    MOV   R2, #1
    MOV   R3, #1        ; sum 初始已经加了 fib(1)=1
    MOV   R0, #1        ; i = 1
    MOV   R7, #10
loop:
    ; 判断 i >= 10 就 halt (用 R6 = R7 - R0)
    SUB   R6, R7, R0
    JZ    R6, done
    ; fib(i+1) = R1 + R2  (存到 R4)
    ADD   R4, R1, R2
    ; 更新: R1 = R2, R2 = R4
    ADD   R1, R2, #0
    ADD   R2, R4, #0
    ; 累加: R3 += R2
    ADD   R3, R3, R2
    ; 计数: R0 += 1
    ADD   R0, R0, #1
    JMP   loop
done:
    HALT
