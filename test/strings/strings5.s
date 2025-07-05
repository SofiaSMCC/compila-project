.data
print_fmt: .string "%d\n"
str_0: .string "hola"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    leaq str_0(%rip), %rax
    leaq str_0(%rip), %rdi
    call puts@PLT
    movq $0, %rax
    jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
