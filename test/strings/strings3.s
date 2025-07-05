.data
print_fmt: .string "%d\n"
str_0: .string "variable"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    leaq str_0(%rip), %rax
    movq %rax, -8(%rbp)
    subq $8, %rsp
    movq $0, %rax
    movq %rax, -16(%rbp)
for_2:
    movq -16(%rbp), %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    movq $10, %rax
    cmpq %rax, %rcx
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je endfor_3
    movq -8(%rbp), %rax
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
    movq -16(%rbp), %rcx
    movq -16(%rbp), %rax
    addq $1, %rax
    movq %rax, -16(%rbp)
    jmp for_2
endfor_3:
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
