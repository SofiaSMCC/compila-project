.data
print_fmt: .string "%d\n"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    movq $0, -8(%rbp)
    movq $1, %rax
    movq %rax, -8(%rbp)
    subq $8, %rsp
    movq $0, -16(%rbp)
    movq $2, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rcx
    movq $5, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    movq $1, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    setg %al
    movzbq %al, %rax
    movq -8(%rbp), %rax
    movq %rax, %rcx
    movq -16(%rbp), %rax
    imulq %rcx, %rax
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
endif_3:
endif_2:
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
