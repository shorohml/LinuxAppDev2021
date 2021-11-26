test "`echo 'md5 md5.sh' | ./rhasher | sed -n 2p`" == "`md5sum ./md5.sh | cut -d ' ' -f1`"
