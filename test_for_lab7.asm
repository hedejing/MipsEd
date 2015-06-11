#BaseAddre:00001000

	add $s6,$0,$0			// clear $s6
	addi $t0,$0,-10			// $t0 = -10 ( = 0xFFFFFFF6)
	addi $t1,$0,20			// $t1 = 20
	slt $t2,$t0,$t1			// $t2 = 1 if $t0 < $t1 (it is!)
	add $s6,$s6,$t2			// $s6 = 1 now if all is well
	slt $t3,$t0,$t1
	xori $t3,$t3,0xffff		// $t3 = 1 if $t0 < $t1 unsigned (not!)
	add $s6,$s6,$t3			// $s6 = 1 still if all is well
	slti $t4,$t0,-5			// $t4 = 1 if $t0 < -5 (it is!)
	add $s6,$s6,$t4			// $s6 = 2 now if all is well
	add $s6, $s6, $s6
	slti $t5,$t0,-5		// $t5 = 1 if $t0 < -5 (yes!)
	add $s6,$s6,$t5			// $s6 = 5 still if all is well
	addi $s6,$s6,1			// $s6 = 6 if all is well

#DataAddre: 0002000; 
var1: 
dd 0;
var2:
dd 0;
var3:
dd 0,0;
