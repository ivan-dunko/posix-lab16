main: main.o service.o list.o
	gcc main.o service.o list.o -o main -pthread -std=c99 -Wall

main.o: main.c list.h service.h
	gcc main.c -c -o main.o -pthread -std=c99 -Wall

list.o: list.c list.h service.h
	gcc list.c -c -o list.o -pthread -std=c99 -Wall

service.o: service.c service.h
	gcc service.c -c -pthread -std=c99 -Wall

clean:
	rm -f main.o service.o list.o


