    .globl  main
    .type   main, @function
main:
    movl    $2foo, %eax
    ret
	  .section    .note.GNU-stack,"",@progbits
