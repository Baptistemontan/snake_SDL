# Makefile ou makefile est un fichier qui permet des exécutions, 
# telle la compilation d'un projet
# C'est un ensemble de règles, où chaque règle est de la forme:
 
#      cible: dépendances
#   		commandes
 
# Notons que chaque commande doit être précédée d'une tabulation

# dans Makefile, il existe plusieurs variables internes telles
# $@ Pour le nom de la cible
# $< Pour le nom de la première dépendance 
# $ˆ Pour la liste des dépendances 
# $? Pour la liste des dépendances plus récentes que la cible 
# $* Pour le nom du fichier, mais sans son suffixe.

FOL = build
CFOL = src
EXEC = exec
CC = gcc
CFLAGS=-O3 #Option d'optimisation du programme
SDLFLAG = `sdl-config --cflags --libs`
FLAGS = -lSDL_image -lpthread
SRC= $(CFOL)/main.c $(CFOL)/fonctions.c
OBJ= $(FOL)/main.o $(FOL)/fonctions.o


all : $(FOL) $(FOL)/$(EXEC)
	./$(FOL)/$(EXEC)

$(FOL)/$(EXEC) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(SDLFLAG) $(FLAGS)

$(FOL)/%.o : $(CFOL)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(SDLFLAG) $(FLAGS)

$(FOL) :
	mkdir $(FOL)
	
clean :
	rm -rf $(FOL)/*.o

rmproper :
	rm -r $(FOL)
