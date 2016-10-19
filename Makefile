compiler: main.c compiler.h base.c parser_helper.c code_gen.c code_gen.h
	gcc main.c base.c parser_helper.c code_gen.c -o compiler
