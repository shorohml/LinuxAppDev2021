CFLAGS = -Wall -Wextra -O0 -g

bin_PROGRAMS=rhasher

rhasher_SOURCES=src/rhasher.c

TESTS=sha1.sh md5.sh

sha1.sh:
	echo 'test "`echo 'sha1 sha1.sh' | ./rhasher | sed -n 2p`" == "`sha1sum ./sha1.sh | cut -d ' ' -f1`"' > $@
	chmod +x $@

md5.sh:
	echo 'test "`echo 'md5 md5.sh' | ./rhasher | sed -n 2p`" == "`md5sum ./md5.sh | cut -d ' ' -f1`"' > $@
	chmod +x $@

gitclean:
	git clean -df