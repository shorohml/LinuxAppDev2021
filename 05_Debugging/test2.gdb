set pagination off

set $var = 0

break range.c:29 if ++$var > 27 && $var < 36
    command 1
    echo @@@
    print m
    echo @@@
    print n
    echo @@@
    print s
    echo @@@
    print i
    continue
end

run

quit