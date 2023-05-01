CC = g++
CONSERVATIVE_FLAGS = -Wall -Wextra -pedantic -std=c++11
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

csim : main.o cache.o
	$(CC) -o csim main.o cache.o

cache.o : cache.cpp cache.h
	$(CC) $(CFLAGS) -c cache.cpp

main.o : main.cpp cache.h
	$(CC) $(CFLAGS) -c main.cpp

solution.zip :
	rm -f solution.zip
	zip -9r $@ Makefile main.cpp cache.cpp cache.h README.txt

clean :
	rm -f csim *.o solution.zip
