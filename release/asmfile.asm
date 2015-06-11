#BaseAddre:00000000
		lui $v1, 0xf000
		addi $s4, $zero, 63
		lui $t0, 0x8000
		add $a0, $v1, $v1
		addi $v0, $zero, 1
		nor $at, $zero, $zero
		add $t2, $at, $zero
		addi $a3, $zero, 3
		nor $a3, $a3, $a3
		addi $a2, $zero, 32767
		add $s1, $zero, $zero
		addi $a1, $zero, 683
		sw $a1, 0($v1)
		addi $s2, $zero, 2
		sw $zero, 4($v1)
		lw $a1, 0($v1)
		add $a1, $a1, $a1
		add $a1, $a1, $a1
		sw $a1, 0($v1)
		sw $a2, 4($v1)
		lui $t5, 0xffff
L5 : 	lw $a1, 0($v1)
		add $a1, $a1, $a1
		add $a1, $a1, $a1
		sw $a1, 0($v1)
		lw $a1, 0($v1)
		and $t3, $a1, $t0
		addi $t5, $t5, 1
		beq $t3, $t0, L1
L11 : 	lw $a1, 0($v1)
		addi $s2, $zero, 24
		and $t3, $a1, $s2
		beq $t3, $zero, L2
		beq $t3, $s2, L3
		addi $s2, $zero, 8
		beq $t3, $s2, L4
		sw $t1, 0($a0)
		j L5
L2 : 	beq $t2, $at, L6
		j L7
L6 : 	nor $t2, $zero, $zero
		add $t2, $t2, $t2
L7 : 	sw $t2, 0($a0)
		j L5
L3 : 	lw $t1, 672($s1)
		sw $t1, 0($a0)
		j L5
L4 : 	lw $t1, 608($s1)
		sw $t1, 0($a0)
		j L5
L1 : 	lui $t5, 0xffff
		add $t2, $t2, $t2
		or $t2, $t2, $v0
		addi $s1, $s1, 4
		and $s1, $s1, $s4
		addi $t1, $t1, 1
		beq $t1, $at, L8
		j L9
L8 : 	addi $t1, $t1, 5
L9 : 	lw $a1, 0($v1)
		add $t3, $a1, $a1
		add $t3, $t3, $t3
		sw $t3, 0($v1)
		sw $a2, 4($v1)
L10 : 	lw $a1, 0($v1)
		and $t3, $a1, $t0
		beq $t3, $t0, L10
		j L11
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
		add $0, $0, $0
