.RECIPEPREFIX = ~
GENERATES = move
TRASH = test_copy.txt test_out.txt

all: move

move: move.c
~      cc -g -O0 $^ -o $@

test: move
~     @echo "\nno arguments:\n"
~     ./move || [ $$? -eq 255 ]
~
~     @echo "\none arguments:\n"
~     ./move test.txt || [ $$? -eq 255 ]
~
~     @echo "\nsame file 1:\n"
~     ./move test.txt test.txt || [ $$? -eq 255 ]
~
~     @echo "\nsame file 2:\n"
~     ./move test.txt ./test.txt || [ $$? -eq 255 ]
~
~     @echo "\noutput is a directory:\n"
~     ./move test.txt ../ || [ $$? -eq 21 ]
~
~     @echo "\nopen strace injection for input file:\n"
~     cp test.txt test_copy.txt
~     strace -P test_copy.txt -eopenat -e fault=openat:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt ) && [ ! -f test_out.txt ]
~     rm test_copy.txt
~
~     @echo "\nopen strace injection for output file:\n"
~     cp test.txt test_copy.txt
~     strace -P test_out.txt -eopenat -e fault=openat:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt ) && [ ! -f test_out.txt ]
~     rm test_copy.txt
~
~     @echo "\nread strace injection:\n"
~     cp test.txt test_copy.txt
~     strace -P test_copy.txt -eread -e fault=read:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt ) && [ ! -f test_out.txt ]
~     rm test_copy.txt
~
~     @echo "\nwrite strace injection:\n"
~     cp test.txt test_copy.txt
~     touch test_out.txt # have to do this or -P won't work
~     strace -P test_out.txt -e fault=write:error=EPERM ./move test_copy.txt test_out.txt || [ $$? -eq 1 ] && ( cmp test_copy.txt test.txt )
~     rm test_copy.txt
~     rm test_out.txt

clean:
~      rm -f $(TRASH)

distclean: clean
~          rm -rf $(GENERATES)