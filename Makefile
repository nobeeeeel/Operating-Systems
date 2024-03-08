all: shell

shell:
	gcc -std=c99 -Wall -pedantic main.c scanner/scanner.c parser/shell.c executor/executor.c parser/parserAux.c -o shell

clean:
	rm -f *~
	rm -f *.o
	rm -f shell

zip:
	zip -r clean.zip * -x "folder/*" -x "folder" -x *.zip