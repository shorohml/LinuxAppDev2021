test "`echo 'sha1 sha1.sh' | ./rhasher | sed -n 2p`" == "`sha1sum ./sha1.sh | cut -d ' ' -f1`"
