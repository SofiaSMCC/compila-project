.data
print_fmt: .string "%d\n"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $40, %rsp
    movq $0, -8(%rbp)
    movq $0, -16(%rbp)
    movq $0, -24(%rbp)
    movq $0, -32(%rbp)
    movq $0, -40(%rbp)
    subq $8, %rsp
    movq $0, -48(%rbp)
    movq $0, %rax
    movq %rax, -48(%rbp)
    movq $0, %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq $10, %rax
    movq %rax, (%rbp, %rdx)
    movq $1, %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq $20, %rax
    movq %rax, (%rbp, %rdx)
    movq $2, %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq $30, %rax
    movq %rax, (%rbp, %rdx)
    movq $3, %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq $40, %rax
    movq %rax, (%rbp, %rdx)
    movq $4, %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq $50, %rax
    movq %rax, (%rbp, %rdx)
    subq $8, %rsp
    movq $0, %rax
    movq %rax, -56(%rbp)
for_4:
    movq -56(%rbp), %rax
    movq -56(%rbp), %rax
    movq %rax, %rcx
    movq $5, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je endfor_5
    movq -56(%rbp), %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq -56(%rbp), %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq (%rbp, %rdx), %rax
    movq %rax, %rcx
    movq $2, %rax
    imulq %rcx, %rax
    movq %rax, (%rbp, %rdx)
    movq -56(%rbp), %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq (%rbp, %rdx), %rax
    movq %rsp, %rcx
    andq $0xf, %rcx
    cmpq $0, %rcx
    je .aligned_printf_6
    subq $8, %rsp
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    addq $8, %rsp
    jmp .after_printf_6
.aligned_printf_6:
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.after_printf_6:
    movq -56(%rbp), %rcx
    movq -56(%rbp), %rax
    addq $1, %rax
    movq %rax, -56(%rbp)
    jmp for_4
endfor_5:
    subq $8, %rsp
    movq $0, %rax
    movq %rax, -56(%rbp)
for_7:
    movq -56(%rbp), %rax
    movq -56(%rbp), %rax
    movq %rax, %rcx
    movq $5, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je endfor_8
    movq -48(%rbp), %rax
    movq %rax, %rcx
    movq -56(%rbp), %rax
    movq %rax, %rdx
    negq %rdx
    imulq $8, %rdx
    addq $-8, %rdx
    movq (%rbp, %rdx), %rax
    addq %rcx, %rax
    movq %rax, -48(%rbp)
    movq -56(%rbp), %rcx
    movq -56(%rbp), %rax
    addq $1, %rax
    movq %rax, -56(%rbp)
    jmp for_7
endfor_8:
    movq -48(%rbp), %rax
    movq %rsp, %rcx
    andq $0xf, %rcx
    cmpq $0, %rcx
    je .aligned_printf_9
    subq $8, %rsp
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    addq $8, %rsp
    jmp .after_printf_9
.aligned_printf_9:
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.after_printf_9:
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
