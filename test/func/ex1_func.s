.data
print_fmt: .string "%d\n"
.text
.globl main
combina:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq %rdx, -24(%rbp)
    subq $8, %rsp
    movq $0, -32(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rcx
    movq -16(%rbp), %rax
    addq %rcx, %rax
    movq %rax, -32(%rbp)
    subq $8, %rsp
    movq $0, -40(%rbp)
    movq -32(%rbp), %rax
    movq %rax, %rcx
    movq -24(%rbp), %rax
    addq %rcx, %rax
    movq %rax, -40(%rbp)
    movq -40(%rbp), %rax
    movq %rax, %rcx
    movq $2, %rax
    imulq %rcx, %rax
    jmp .end_combina
.end_combina:
leave
ret
main:
    pushq %rbp
    movq %rsp, %rbp
    movq $1, %rax
    movq %rax, %rdi
    movq $2, %rax
    movq %rax, %rsi
    movq $3, %rax
    movq %rax, %rdx
    call combina
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
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
