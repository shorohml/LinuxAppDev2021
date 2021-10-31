.RECIPEPREFIX = ~
GENERATES = move
TRASH = protected_remove.so test_copy.txt test_out.txt test_PROTECTED.txt

all: move

protected_remove: protected_remove.c
~                 cc $^ -shared -o $@.so

move: move.c
~      cc -g -O0 $^ -o $@

test: move protected_remove
~     cp test.txt test_copy.txt
~
~     @echo "\n no arguments:\n"
~     ./move || [ $$? -eq 255 ]
~
~     @echo "\n one arguments:\n"
~     ./move test_copy.txt || [ $$? -eq 255 ]
~
~     @echo "\n same file 1:\n"
~     ./move test_copy.txt test_copy.txt || [ $$? -eq 255 ]
~
~     @echo "\n same file 2:\n"
~     ./move test_copy.txt ./test_copy.txt || [ $$? -eq 255 ]
~
~     @echo "\n output is a directory:\n"
~     ./move test_copy.txt ../ || [ $$? -eq 21 ]
~
~     @echo "\n actual move:\n"
~     ./move test_copy.txt test_out.txt && cmp test_out.txt test.txt
~     rm test_out.txt
~
~     @echo "\n open strace injection for input file:\n"
~     cp test.txt test_copy.txt
~     strace -P test_copy.txt -eopenat -e fault=openat:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt ) && [ ! -f test_out.txt ]
~
~     @echo "\n open strace injection for output file:\n"
~     cp test.txt test_copy.txt
~     strace -P test_out.txt -eopenat -e fault=openat:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt ) && [ ! -f test_out.txt ]
~
~     @echo "\n read strace injection:\n"
~     cp test.txt test_copy.txt
~     strace -P test_copy.txt -eread -e fault=read:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt ) && [ ! -f test_out.txt ]
~
~     @echo "\n write strace injection:\n"
~     cp test.txt test_copy.txt
~     touch test_out.txt # have to do this or -P won't work
~     strace -P test_out.txt -e fault=write:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt )
~
~     @echo "\n protected remove:\n"
~     cp test.txt test_PROTECTED.txt
~     LD_PRELOAD=protected_remove.so ./move test_PROTECTED.txt test_out.txt && cmp test_out.txt test.txt && [ -f test_PROTECTED.txt ]
~
~     rm -f $(TRASH)

clean:
~      rm -f $(TRASH)

distclean: clean
~          rm -rf $(GENERATES)