.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

addgf:
  subu $sp, $sp, 56
  sw $ra, 52($sp)
  sw $fp, 48($sp)
  addi $fp, $sp, 56
  sw $a0, -12($fp)
  sw $a1, -16($fp)
  sw $a2, -20($fp)
  sw $a3, -24($fp)
  lw $t0, 0($fp)
  sw $t0, -28($fp)
  lw $t0, 4($fp)
  sw $t0, -32($fp)
  lw $t0, 8($fp)
  sw $t0, -36($fp)
  lw $t1, -16($fp)
  lw $t2, -20($fp)
  mul $t0, $t1, $t2
  sw $t0, -40($fp)
  lw $t1, -12($fp)
  lw $t2, -40($fp)
  sub $t0, $t1, $t2
  sw $t0, -44($fp)
  lw $t1, -28($fp)
  lw $t2, -32($fp)
  mul $t0, $t1, $t2
  sw $t0, -48($fp)
  lw $t1, -44($fp)
  lw $t2, -48($fp)
  add $t0, $t1, $t2
  sw $t0, -52($fp)
  lw $t1, -52($fp)
  lw $t2, -36($fp)
  add $t0, $t1, $t2
  sw $t0, -56($fp)
  lw $v0, -56($fp)
  lw $ra, 52($sp)
  lw $fp, 48($sp)
  addi $sp, $sp, 56
  jr $ra
  li $v0, -1
  lw $ra, 52($sp)
  lw $fp, 48($sp)
  addi $sp, $sp, 56
  jr $ra

main:
  subu $sp, $sp, 40
  sw $ra, 36($sp)
  sw $fp, 32($sp)
  addi $fp, $sp, 40
  li $t0, 1
  sw $t0, -12($fp)
  li $t0, 1
  sw $t0, -16($fp)
  li $t0, 3
  sw $t0, -12($fp)
  jal read
  sw $v0, -20($fp)
  lw $t1, -16($fp)
  addi $t0, $t1, 1
  sw $t0, -24($fp)
  lw $t1, -20($fp)
  addi $t0, $t1, 1
  sw $t0, -28($fp)
  lw $t1, -12($fp)
  addi $t0, $t1, 1
  sw $t0, -32($fp)
  subu $sp, $sp, 12
  li $t0, 8
  sw $t0, 8($sp)
  li $t0, 7
  sw $t0, 4($sp)
  li $t0, 6
  sw $t0, 0($sp)
  li $a3, 5
  lw $a2, -32($fp)
  lw $a1, -28($fp)
  lw $a0, -24($fp)
  jal addgf
  sw $v0, -36($fp)
  addi $sp, $sp, 12
  lw $t1, -36($fp)
  addi $t0, $t1, 1
  sw $t0, -40($fp)
  lw $a0, -40($fp)
  jal write
  lw $v0, -12($fp)
  lw $ra, 36($sp)
  lw $fp, 32($sp)
  addi $sp, $sp, 40
  jr $ra
