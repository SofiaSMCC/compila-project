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
    movq $1, %rax
    movq %rax, -8(%rbp)
    movq $2, %rax
    movq %rax, -16(%rbp)
    movq $3, %rax
    movq %rax, -24(%rbp)
    movq $4, %rax
    movq %rax, -32(%rbp)
    movq $5, %rax
    movq %rax, -40(%rbp)
    subq $8, %rsp
    movq $0, -48(%rbp)
    movq $0, %rax
    movq %rax, -48(%rbp)
    subq $8, %rsp
    movq $0, %rax
    movq %rax, -56(%rbp)
for_2:
    movq -56(%rbp), %rax
    movq -56(%rbp), %rax
    movq %rax, %rcx
    movq $5, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je endfor_3
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
    jmp for_2
endfor_3:
    movq -48(%rbp), %rax
    movq %rsp, %rcx
    andq $0xf, %rcx
    cmpq $0, %rcx
    je .aligned_printf_4
    subq $8, %rsp
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    addq $8, %rsp
    jmp .after_printf_4
.aligned_printf_4:
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.after_printf_4:
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
