#!/usr/bin/bash

check_file() {
    local infile=$1
    local outfile=$(echo $infile | sed 's/in/out/g')
    local propfile=$(echo $outfile | sed 's/tests/tests\/proper/g')
    printf "%-32s" "Test for file $infile "
#   local TIME=$( { TIMEFORMAT='%3Rs %M'; time build/Release/src/siano $infile; } 2>&1 )
    local TIME=$( { /usr/bin/time -f "%e sec, %M Kbytes" ./build/Release/src/siano $infile; } 2>&1 )
    local DIFF=$(diff $outfile $propfile)
    if [ "$DIFF" != "" ]
    then
        echo "[FAILED]"
        return 1
    else
        echo "[  OK  ] $TIME"
    fi
    return 0
}

if [ $# -eq 0 ]
then
    for f in tests/*in
    do
        check_file $f
        if [ "$?" -ne 0 ]
        then
            break
        fi
    done
else
    check_file $1
fi

