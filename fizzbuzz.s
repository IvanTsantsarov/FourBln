##   Fizzbuzz*
##   The standard fizzbuzz task, with a catch:
##   Write a program that outputs the numbers from 1 to 100. If the number 
##   is divisible by 3, instead of it print “A”, if it’s divisible by 5 - “B”
##   if it’s divisible by 15 - “AB”. The catch is to write it without
##   any conditionals/ifs, including for/while loops.

.section .rodata
    fmt_a:   .string "A\n"
    fmt_b:   .string "B\n"
    fmt_ab:  .string "AB\n"

     string_table:
        .quad fmt_a     # Index 1
        .quad fmt_b     # Index 2
        .quad fmt_ab    # Index 3

.section .text
.global Fizzbuzz

Fizzbuzz:
    # Initialize rbx with 100 for a countdown, or 1 for countup.
    # To avoid 'if', we use a recursive approach where the function
    # pointers in a table determine the next action.
    push %rbp
    mov %rsp, %rbp
    movq $1, %rcx       # Start at 1
    call main_loop
    pop %rbp
    ret


# print from the offset in the string table stored in rbx
print_letter:
    push %rcx           # Save counter

    # get string table address and load format string based on rbx
    lea string_table(%rip), %rax
    movq (%rax, %rbx, 8), %rdi 

    # 2. Call printf
    movq %rbx, %rsi     # Second arg for %ld
    xor %eax, %eax      # clear floating point args
    call printf@PLT     # call printf
    pop %rcx
    ret

    # 3. Increment and continue WITHOUT 'jne'
main_loop:    
    incq %rcx

    mov $0, %rbx
    call print_letter

    mov $1, %rbx
    call print_letter

    mov $2, %rbx
    call print_letter

    ret

# This tells Linux the stack is not executable
.section .note.GNU-stack,"",@progbits
