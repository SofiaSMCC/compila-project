.data
print_fmt: .string "%d\n"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    movq $0, -8(%rbp)
    movq $-2, %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rcx
    movq $0, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    setg %al
    movzbq %al, %rax
    movq -8(%rbp), %rax
    movq %rsp, %rcx
    andq $0xf, %rcx
    cmpq $0, %rcx
    je .aligned_printf_3
    subq $8, %rsp
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    addq $8, %rsp
    jmp .after_printf_3
.aligned_printf_3:
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.after_printf_3:
endif_2:
    movq -8(%rbp), %rax
    movq %rax, %rcx
    movq $2, %rax
    imulq %rcx, %rax
    movq %rax, %rcx
    movq $-4, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    sete %al
    movzbq %al, %rax
    movq $2, %rax
    movq %rax, %rcx
    movq -8(%rbp), %rax
    imulq %rcx, %rax
    movq %rsp, %rcx
    andq $0xf, %rcx
    cmpq $0, %rcx
    je .aligned_printf_5
    subq $8, %rsp
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    addq $8, %rsp
    jmp .after_printf_5
.aligned_printf_5:
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.after_printf_5:
endif_4:
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
