#!/bin/bash

SCRIPT_DIR="./func_tests/scripts"

if [ $# -ne 3 ]; then
    exit 2
fi

POS_IN="$1"
POS_EXPECT="$2"
POS_ARGS="$3"

{
    args_arr=($(cat "$POS_ARGS"))

    "./app.exe" "${args_arr[@]}" < "$POS_IN" > "/tmp/pos_out_file.txt"
    if [ $? -ne 0 ]; then
        exit 1
    fi

    "$SCRIPT_DIR/comparator.sh" "/tmp/pos_out_file.txt" "$POS_EXPECT"
    exit $?

} > /dev/null
