section .data

massive db 256 dup (0)

; --------------------------------------------------------------------------------

section .rodata

num_mas db "0123456789"
superconst_10 dq 10.0
superconst_67 dq 0x7FFFFFFFFFFFFFFF

; ---------------------------------------------------------------------------------

section .text
global my_printf_float

my_printf_float:
                    push r9 
                    push r8
                    push rcx
                    push rdx
                    push rsi

.int_part:          
                    movsd xmm15, xmm14                  ;number in xmm14
                    roundsd xmm15, xmm15, 3

                    cvttsd2si rax, xmm15
                    mov rsi, massive                    ;address of outcoming string
                    xor rdx, rdx                        ;len + address in buffer

                    movq rdi, xmm14
                    shr rdi, 63
                    cmp rdi, 1
                    jne .not_negative

                    mov r10, '-'
                    mov [rsi], r10b
                    inc rdx                             ;len + 1
                    call .print_printf

                    neg rax

.not_negative:      
                    mov rcx, 20
                    mov r8, 10

.cvt_int_in_str:               
                    dec rcx
                    div r8
                    movzx rdx, byte [num_mas + rdx]
                    mov [rsi + rcx], dl
                    xor rdx, rdx

                    cmp rcx, 0
                    jne .cvt_int_in_str
                
                    mov rdx, 20
                    xor rcx, rcx

.delete_zeros_int:  cmp byte [rsi + rcx], '0'
                    jne .end_int
                    cmp rcx, 19
                    je .end_int

                    inc rcx
                    dec rdx
                    jmp .delete_zeros_int


.end_int:           push rsi
                    add rsi, rcx
                    call .print_printf
                    pop rsi

; ----------------------------

.fract_part:        mov r10, '.'
                    mov [rsi], r10b
                    inc rdx                             ;len + 1
                    call .print_printf

                    movq rax, xmm14                     ; xmm14 -> |xmm14|
                    and rax, [superconst_67]
                    movq xmm14, rax

                    movq rax, xmm15                     ; xmm15 -> |xmm15|
                    and rax, [superconst_67]
                    movq xmm15, rax

                    xor rcx, rcx

.cvt_frac_in_str:
                    subsd xmm14, xmm15
                    mulsd xmm14, [superconst_10]
                    movsd xmm15, xmm14
                    roundsd xmm15, xmm15, 3

                    cvttsd2si rdx, xmm15

                    movzx rdx, byte [num_mas + rdx]
                    mov [rsi + rcx], dl
                    xor rdx, rdx

                    inc rcx
                    cmp rcx, 16
                    jne .cvt_frac_in_str

                    mov rdx, 16
                    mov rcx, 15

.delete_zeros_frac: 
                    cmp byte [rsi + rcx], '0'
                    jne .end_frac
                    cmp rcx, 0
                    je .end_frac

                    dec rcx
                    dec rdx
                    jmp .delete_zeros_frac

.end_frac:          call .print_printf

                    mov r10, 10                         ;\n
                    mov [rsi], r10b
                    inc rdx                             ;len + 1
                    call .print_printf


                    jmp .exit_printf



;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.print_printf:      push rax
                    push rcx
                    mov rax, 1                          ;func write
                    mov rdi, 1                          ;stdout
                    syscall
                    pop rcx
                    pop rax
                    xor rdx, rdx
                    ret

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++

.exit_printf:
                    pop rsi
                    pop rdx
                    pop rcx
                    pop r8
                    pop r9
                    ret

; ----------------------------------------------------------------------------------------