# void mpxtraverse(void* src, size_t sz)

.globl mpxtraverse
mpxtraverse:
  push %rbp
  mov %rsp, %rbp
  push %rax
  push %rcx
  push %rdx
  push %r8
  push %r9

# Calculate and store buffer locations
# %bnd0.lower: buffer start address
# %bnd1.lower: buffer midpoint
  shr $1, %rsi
  mov %rdi, %rdx
  mov %rsi, %rax
  sub $0x1, %rax
  bndmk (%rdx,%rax,1),%bnd0
  add %rsi, %rdx
  bndmk (%rdx,%rax,1),%bnd1

  # Clobber loop variables, begin the loop
  xor %rax, %rax
  xor %rcx, %rcx
  xor %rdx, %rdx
  jmp continue

continue:
# %rax: fetched value from lower half of buffer
# %rcx: fetched value from upper half of buffer
# %rdx: counter value

  # Move from bounds registers onto stack
  bndmov %bnd0, -0x10(%rsp)
  bndmov %bnd1, -0x20(%rsp)

  # Move bounds from stack into GP registers
  mov -0x10(%rsp), %rax
  mov -0x20(%rsp), %rcx

  # Sanitize values written onto the stack
  andq $0, -0x08(%rsp)
  andq $0, -0x10(%rsp)
  andq $0, -0x18(%rsp)
  andq $0, -0x20(%rsp)

  # Read values from memory, XOR them to simulate "unhiding"
  mov (%rax, %rdx, 1), %r8
  mov (%rcx, %rdx, 1), %r9
  xor %r8, %r9

  # Increment the counter
  inc %rdx

  # %rax + %rdx == %rcx: exit, else: continue
  add %rdx, %rax
  cmp %rcx, %rax
  je exit
  jmp continue

exit:
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  pop %rax
  pop %rbp
  ret
