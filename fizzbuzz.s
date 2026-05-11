# task.s
.section .rodata
    fmt: .string "Counter: %ld\n"

.section .text
.global taskFizzbuzz    # Make the function visible to the linker

taskFizzbuzz:
    push %rbx           # rbx is callee-saved; we MUST save it if we use it
    movq $0, %rbx       # Initialize counter

begin_loop:
    # Set up arguments for printf
    lea fmt(%rip), %rdi # Load address of fmt string into rdi
    movq %rbx, %rsi     # Move counter to rsi
    xorl %eax, %eax     # No floating point args
    call printf@PLT     # Call printf

    incq %rbx           # Increment counter
    cmpq $10, %rbx      # Compare
    jne begin_loop      # Loop if not 10

    pop %rbx            # Restore the original rbx value
    ret                 # Return to C++