.data
print_fmt: .string "%d\n"
.text
.globl main
imprimesuma:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    subq $8, %rsp
    movq $0, -24(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rcx
    movq -16(%rbp), %rax
    addq %rcx, %rax
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rax
    movq %rsp, %rcx
    andq $0xf, %rcx
    cmpq $0, %rcx
    je .aligned_printf_0
    subq $8, %rsp
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    addq $8, %rsp
    jmp .after_printf_0
.aligned_printf_0:
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.after_printf_0:
.end_imprimesuma:
leave
ret
main:
    pushq %rbp
    movq %rsp, %rbp
    movq $3, %rax
    movq %rax, %rdi
    movq $4, %rax
    movq %rax, %rsi
    call imprimesuma
    movq $10, %rax
    movq %rax, %rdi
    movq $20, %rax
    movq %rax, %rsi
    call imprimesuma
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
