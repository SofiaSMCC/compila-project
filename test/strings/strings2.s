.data
print_fmt: .string "%d\n"
str_0: .string "Ana"
str_1: .string "Hola!"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    leaq str_0(%rip), %rax
    movq %rax, -8(%rbp)
    subq $8, %rsp
    leaq str_1(%rip), %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax
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
    movq -8(%rbp), %rax
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
