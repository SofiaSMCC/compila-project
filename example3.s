.data
print_fmt: .string "%ld\n"
.text
.globl main
 .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $0, -8(%rbp)
    movq $1, %rax
    movq %rax, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
for_0:
    movq -16(%rbp), %rax
    pushq %rax
    movq $10, %rax
    movq %rax, %rdx
    popq %rax
    cmpq %rdx, %rax
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je endfor_1
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rdx
    popq %rax
    addq %rdx, %rax
    movq %rax, -8(%rbp)
    movq -16(%rbp), %rax
    addq $1, %rax
    movq %rax, -16(%rbp)
    jmp for_0
endfor_1:
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
