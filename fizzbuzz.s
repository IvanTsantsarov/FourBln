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
    fmt_num: .string "%ld\n"

     string_table:
        .quad fmt_a     # Index 1
        .quad fmt_b     # Index 2
        .quad fmt_ab    # Index 3
        .quad fmt_num   # Index 4

.section .text
.global Fizzbuzz

Fizzbuzz:
    # standart prologue
    push %rbp
    mov %rsp, %rbp
    
    # Initialize the counter to 0
    movq $0, %rcx       
    
    call main_loop
    pop %rbp

    # go back to c++ code
    ret


# rax = rcx / rbx
# input: rbx = divisor
# rdx is the reminder
divide:
    mov %rcx, %rax     # Move dividend to rax for division
    xor %rdx, %rdx      # Clear rdx for division
    div %rbx            # rax = rbx / rcx, rdx = rbx % rcx
    ret

# Print a string from string table
# RAX stores the index of the format (0 for A, 1 for B, 2 for AB, 3 for number
# 
print_letter:
    push %rcx          # Save counter
    mov %rax, %rbx     # save index in rbx for later use

    # get string table address and load format string based on rbx
    lea string_table(%rip), %rax
    movq (%rax, %rbx, 8), %rdi 

    # 2. Call printf
    movq %rcx, %rsi     # Second arg for %ld (it's the counter)
    xor %rax, %rax      # clear floating point args
    call printf@PLT     # call printf
    pop %rcx
    ret


main_loop:    

    incq %rcx

    mov $100, %rbx
    call divide

    ## if counter is greater than 100, exit
    lea jump_map_exit(%rip), %rbx
    shlq $3, %rax
    add %rax, %rbx
    jmp *%rbx

less_than_100:
    ############################ 
    ## divide counter by 15
    mov $15, %rbx
    call divide

    # set rbx to print AB
    mov $2, %rax
    
    # calculate address based on the reminder and jump_map label
    lea jump_map(%rip), %rbx
    shlq $1, %rdx
    add %rdx, %rbx
    
    # set continue pointer into rdx
    lea check_3(%rip), %rdx
    
    jmp *%rbx

check_3:
    ############################ 
    ## divide counter by 3
    mov $3, %rbx
    call divide

    # set rbx to print A
    mov $0, %rax
    
    # jump to the address based on the reminder
    lea jump_map(%rip), %rbx
    shlq $1, %rdx
    add %rdx, %rbx
    

    # set continue pointer into rdx
    lea check_5(%rip), %rdx
    jmp *%rbx

check_5:
    ############################ 
    ## divide counter by 5
    mov $5, %rbx
    call divide

    # set rbx to print B
    mov $1, %rax

    # jump to the address based on the reminder
    lea jump_map(%rip), %rbx
    shlq $1, %rdx
    add %rdx, %rbx
    

    # set continue pointer into rdx
    lea print_counter(%rip), %rdx
    jmp *%rbx

print_counter:
    # print current counter if it's not divisible by 3, 5 or 15
    mov $3, %eax
    call print_letter
    jmp main_loop

print_on_devided:
    # print current counter if it's not divisible by 3, 5 or 15
    call print_letter
    jmp main_loop

jump_map:
    jmp print_on_devided # zero reminder

    # jump in the field of nothing if any reminder
    # biggest reminder of 100 / 3, 5 or 15 is 14, so we need at least 15 nops
    # multiplied by 2 because we are using 2 bytes for each jump (shlq $1, %rdx)
    # here we have 30 nops:
    nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
    nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
    nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
    
    # continue testing
    jmp *%rdx

jump_map_exit:
    jmp less_than_100 # zero reminder
    nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
    ret # return to c++ code


# This tells Linux the stack is not executable
.section .note.GNU-stack,"",@progbits
