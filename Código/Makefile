CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -pthread
OBJ=funciones.o lab3.o
DEPS=funciones.h
EXEC=lab3

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ -lm

funciones.o: funciones.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

lab3.o: lab3.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)