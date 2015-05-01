CC=gcc
FLAGS=-lpthread -rdynamic -ldl -lpcre -g -W -Wall
EXEC=cherche

all: $(EXEC) clean

cherche: cherche.o liste.o file.o semaphore.o search.o
	$(CC) -g -o $@ $^ $(FLAGS)

cherche.o: cherche.c
	$(CC) -g -o $@ -c $<
	
liste.o: liste.c
	$(CC) -g -o $@ -c $<
	
file.o: file.c
	$(CC) -g -o $@ -c $<
	
semaphore.o: semaphore.c
	$(CC) -g -o $@ -c $<

search.o: search.c
	$(CC) -g -o $@ -c $<
	
clean:
	rm -rf *.o
