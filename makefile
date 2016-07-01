main.o: readGraph.c sepm.c spm.c
	gcc -g readGraph.c sepm.c spm.c -o main
dbmain: readGraph.c sepm.c spm.c
	gcc -g -fbounds-check readGraph.c sepm.c spm.c -o dbmain

