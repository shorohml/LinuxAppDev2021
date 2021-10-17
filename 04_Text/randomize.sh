DELAY=${1:-0.0}

hexdump -v -e '"%_ad " /1 "%x\n"' > dump.txt
cat dump.txt | sed -e '/[0-9]\+ a$/!d' | cut -d ' ' -f1 > sep.txt
cat dump.txt | sed -e '/[0-9]\+ a$/d'  | shuf > dump_shuf.txt
NUM_LINES="$(cat sep.txt | wc -l)"

tput clear
while read LINE; do
    IDX="$(cut -d' ' -f1 <<<"$LINE")"
    SMB="$(cut -d' ' -f2 <<<"$LINE")"

    if [[ $SMB == "20" ]]; then
        continue
    fi

    LINE_IDX=0
    START_IDX=0
    while read SEP_IDX; do
        if [[ $SEP_IDX -le $IDX ]]; then
            (( LINE_IDX = LINE_IDX + 1 ))
            START_IDX=$SEP_IDX
        fi
    done <sep.txt
    (( COL_IDX = IDX - START_IDX ))

    sleep $DELAY
    tput cup $LINE_IDX $COL_IDX; /bin/echo -e "\\x$SMB"
done <dump_shuf.txt
tput cup $NUM_LINES 0

rm ./dump.txt
rm ./dump_shuf.txt
rm ./sep.txt