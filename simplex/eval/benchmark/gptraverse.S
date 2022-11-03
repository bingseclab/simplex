# void gptraverse(void* src, size_t sz)

.globl gptraverse
gptraverse:
  push %rbp
  mov %rsp, %rbp
  push %rax
  push %rdx
  push %rcx
  push %r8
  push %r9


  # Calculate and store buffer locations
  # %rdi: buffer start address
  # %rsi: buffer midpoint
  shr $1, %rsi
  add %rdi, %rsi

  # Clobber loop variables, begin the loop
  xor %rax, %rax
  xor %rcx, %rcx
  xor %rdx, %rdx
  jmp continue

continue:
# %rax: fetched value from lower half of buffer
# %rcx: fetched value from upper half of buffer
# %rdx: counter value

  # Move bounds from arguments into GP registers
  mov %rdi, %rax
  mov %rsi, %rcx

  # Read values from the buffer, XOR them to simulate "unhiding"
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
