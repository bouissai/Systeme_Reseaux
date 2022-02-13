CC=gcc -Wall
BIN=exec
EXEC=$(BIN)/serveur $(BIN)/client

SRCDIR=src

all : $(EXEC)

$(BIN)/serveur : $(SRCDIR)/serveur.c 
	$(CC) $(SRCDIR)/serveur.c -o $(BIN)/serveur

$(BIN)/client : $(SRCDIR)/client.c
	$(CC) $(SRCDIR)/client.c -o $(BIN)/client


clean:
	rm  $(BIN)/serveur 
	rm $(BIN)/client