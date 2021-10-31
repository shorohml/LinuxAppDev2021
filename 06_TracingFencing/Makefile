.RECIPEPREFIX = ~
GENERATES = move
TRASH =

all: move

move: move.c
~      cc -g -O0 $^ -o $@

clean:
~      rm -f $(TRASH)

distclean: clean
~          rm -rf $(GENERATES)