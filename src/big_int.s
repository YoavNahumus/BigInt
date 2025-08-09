.text
.globl big_int_add
big_int_add:
    sub %rcx, %rsi
    xor %rax, %rax
__loop_add:
    mov (%rdx), %r8
    lea 8(%rdx), %rdx
    adc %r8, (%rdi)
    lea 8(%rdi), %rdi
    dec %ecx
    jnz __loop_add
__start_add_carry_loop:
    jnc __end_add_carry_loop
    subq $1, %rsi
    jc __end_add_carry_loop
    addq $1, (%rdi)
    lea 8(%rdi), %rdi
    jmp __start_add_carry_loop
__end_add_carry_loop:
    setc %al
    ret

.globl big_int_sub
big_int_sub:
    sub %rcx, %rsi
    xor %rax, %rax
__loop_sub:
    mov (%rdx), %r8
    lea 8(%rdx), %rdx
    sbb %r8, (%rdi)
    lea 8(%rdi), %rdi
    dec %ecx
    jnz __loop_sub
__start_sub_carry_loop:
    jnc __end_sub_carry_loop
    subq $1, %rsi
    jc __end_sub_carry_loop
    subq $1, (%rdi)
    lea 8(%rdi), %rdi
    jmp __start_sub_carry_loop
__end_sub_carry_loop:
    setc %al
    ret

.section .note.GNU-stack,"",@progbits
