#!/bin/bash

SCRIPT_DIR="./func_tests/scripts"

if [ $# -ne 3 ]; then
    exit 2
fi

NEG_IN="$1"
NEG_EXPECT="$2"
NEG_ARGS="$3"

{
    args_arr=($(cat "$NEG_ARGS"))

    "./app.exe" "${args_arr[@]}" < "$NEG_IN" > "/tmp/neg_out_file.txt"

    "$SCRIPT_DIR/comparator.sh" "/tmp/neg_out_file.txt" "$NEG_EXPECT"
    exit $?

} > /dev/null
