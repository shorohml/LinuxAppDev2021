.RECIPEPREFIX = ~
GENERATES = README liboutput_static.a liboutput.so prog prog-a prog-so
TRASH = *.o *~ o.* *.txt
CFLAGS = -Wall -fPIC
SHELL := /bin/bash

all:    README output_static output prog prog-a prog-so test

output_static:  const.o fun.o
~               ar -rcs lib$@.a $^

output: fun.o const.o
~       cc -shared $^ -o lib$@.so

prog:   const.o fun.o prog.o

prog-a: output_static prog.o
~       cc -L. $(word 2,$^) -l$< -o $@

prog-so:    output prog.o
~           cc -L. $(word 2,$^) -l$< -o $@

README: prog
~       ./$< 2> $@

fun.o:  outlib.h

test:   prog prog-so prog-a
~       exec -a prog ./prog    2>prog-err.txt
~       exec -a prog ./prog-so 2>prog-so-err.txt
~       exec -a prog ./prog-a  2>prog-a-err.txt
~       cmp prog-err.txt prog-so-err.txt && cmp prog-so-err.txt prog-a-err.txt       
~       ./prog     aaa > prog-1.txt
~       ./prog-so  aaa > prog-so-1.txt
~       ./prog-a   aaa > prog-a-1.txt
~       cmp prog-1.txt prog-so-1.txt && cmp prog-so-1.txt prog-a-1.txt
~       ./prog     aaa bbbbb > prog-2.txt
~       ./prog-so  aaa bbbbb > prog-so-2.txt
~       ./prog-a   aaa bbbbb > prog-a-2.txt
~       cmp prog-2.txt prog-so-2.txt && cmp prog-so-2.txt prog-a-2.txt

clean:
~        rm -f $(TRASH)

distclean:  clean
~           rm -rf $(GENERATES)