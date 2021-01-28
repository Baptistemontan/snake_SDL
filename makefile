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
WFOL = Wbuild
CFOL = src
EXEC = snake
WEXEC = snake
CC = gcc
WCC = x86_64-w64-mingw32-gcc
CFLAGS=-O3 #Option d'optimisation du programme
SDLFLAG = `sdl-config --cflags --libs`
FLAGS = -lSDL_image -lpthread
SRC= $(CFOL)/main.c $(CFOL)/fonctions.c $(CFOL)/render.c $(CFOL)/files.c
OBJ= $(FOL)/main.o $(FOL)/fonctions.o $(FOL)/render.o $(FOL)/files.o
WOBJ= $(WFOL)/main.o $(WFOL)/fonctions.o $(WFOL)/render.o $(WFOL)/files.o


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

windows : $(WFOL) $(WFOL)/$(WEXEC)

$(WFOL) :
	mkdir $(WFOL)

$(WFOL)/$(WEXEC) : $(WOBJ)
	$(WCC) $(CFLAGS) -o $@ $^ $(SDLFLAG) $(FLAGS)

$(WFOL)/%.o : $(CFOL)/%.c
	$(WCC) $(CFLAGS) -o $@ -c $< $(SDLFLAG) $(FLAGS)

