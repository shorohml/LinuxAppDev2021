.RECIPEPREFIX = ~
GENERATES = range
TRASH =

all: range

range: range.c
~      cc -g -O0 $^ -o $@

test: range
~     gdb --command=test1.gdb --batch --quiet --args ./range 1 12       | grep ^@@@ | cmp test1.txt
~     gdb --command=test2.gdb --batch --quiet --args ./range -100 100 3 | grep ^@@@ | cmp test2.txt

clean:
~      rm -f $(TRASH)

distclean: clean
~          rm -rf $(GENERATES)