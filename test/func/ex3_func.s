.data
print_fmt: .string "%d\n"
.text
.globl main
cuadrado:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rcx
    movq -8(%rbp), %rax
    imulq %rcx, %rax
    jmp .end_cuadrado
.end_cuadrado:
leave
ret
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    movq $0, -8(%rbp)
    movq $5, %rax
    movq %rax, %rdi
    call cuadrado
    movq %rax, -8(%rbp)
    subq $8, %rsp
    movq $0, -16(%rbp)
    movq $7, %rax
    movq %rax, %rdi
    call cuadrado
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax
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
    movq -16(%rbp), %rax
    movq %rsp, %rcx
    andq $0xf, %rcx
    cmpq $0, %rcx
    je .aligned_printf_1
    subq $8, %rsp
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    addq $8, %rsp
    jmp .after_printf_1
.aligned_printf_1:
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.after_printf_1:
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
