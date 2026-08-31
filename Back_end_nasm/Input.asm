[BITS 64]

section .data

massive db 64 dup (0)
neg_flag db 0

; --------------------------------------------------------------------------------

section .rodata

puperconst_10 dq 10.0
code_0 dq '0'

; ---------------------------------------------------------------------------------

section .text
global my_input_float

my_input_float:
                    push rax
                    push rbx
                    push rcx
                    push rdx
                    push rdi
                    push rsi

                    xorpd xmm14, xmm14

                    mov rax, 0
                    mov rdi, 0
                    mov rsi, massive
                    mov rdx, 64
                    syscall

                    mov rax, [code_0]
                    xor rdi, rdi

                    movzx rbx, byte [rsi + rdi]
                    cmp rbx, '-'
                    jne .count_int

                    mov rdx, 1
                    mov byte [neg_flag], dl
                    inc rdi

.count_int:         movzx rbx, byte [rsi + rdi]
                    inc rdi

                    cmp rbx, 10
                    je .end_count_frac

                    cmp rbx, '.'
                    je .end_count_int

                    sub rbx, [code_0]
                    cvtsi2sd xmm15, rbx
                    mulsd xmm14, [puperconst_10]
                    addsd xmm14, xmm15

                    jmp .count_int
        
.end_count_int:    
                    mov rdx, 1
.count_frac:
                    movzx rbx, byte [rsi + rdi]
                    inc rdi

                    cmp rbx, 10
                    je .end_count_frac

                    sub rbx, '0'
                    cvtsi2sd xmm15, rbx 
                    mov rcx, rdx

.div_num:           divsd xmm15, [puperconst_10]
                    loop .div_num

                    addsd xmm14, xmm15
                    inc rdx
                    jmp .count_frac

.end_count_frac: 
                    movzx rdx, byte [neg_flag]
                    cmp dl, 0
                    je .end_input

                    xorpd xmm15, xmm15
                    subsd xmm15, xmm14
                    movsd xmm14, xmm15

.end_input:
                    pop rsi
                    pop rdi
                    pop rdx
                    pop rcx
                    pop rbx
                    pop rax

                    ret