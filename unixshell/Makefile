all: d8sh

d8sh: d8sh.o lexer.o parser.tab.o executor.o
	gcc -lreadline -o  d8sh d8sh.o lexer.o parser.tab.o executor.o

lexer.o: lexer.c parser.tab.o
	gcc -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all -Wextra -c lexer.c

parser.tab.o: parser.tab.c command.h
	gcc -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all -Wextra -c parser.tab.c

executor.o: executor.c command.h executor.h
	gcc -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all -Wextra -c executor.c

d8sh.o: d8sh.c executor.h lexer.h
	gcc -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all -Wextra -c d8sh.c

clean:
	rm -f *.o
	rm d8sh