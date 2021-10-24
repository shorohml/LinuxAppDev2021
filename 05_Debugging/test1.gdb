set pagination off

break range.c:29 if i % 5 == 0
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