DELAY=${1:-0.0}

hexdump -v -e '"%_ad " /1 "%x\n"' > dump.txt
cat dump.txt | sed -e '/[0-9]\+ a$/!d' | cut -d ' ' -f1 > sep.txt
cat dump.txt | sed -e '/[0-9]\+ a$/d'  | shuf > dump_shuf.txt
cat dump_shuf.txt | cut -d ' ' -f1 > dump_shuf_idx.txt
cat dump_shuf.txt | cut -d ' ' -f2 > dump_shuf_smb.txt
NUM_LINES="$(cat sep.txt | wc -l)"

tput clear
while read IDX <&3 && read SMB <&4; do
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
done 3<dump_shuf_idx.txt 4<dump_shuf_smb.txt
tput cup $NUM_LINES 0

rm ./dump.txt
rm ./dump_shuf.txt
rm ./dump_shuf_idx.txt
rm ./dump_shuf_smb.txt
rm ./sep.txt