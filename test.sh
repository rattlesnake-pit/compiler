./compiler < test | ~/asm/asm_lin > test.chop 2> /dev/null && ~/ModVM/vm test.chop && rm test.chop
