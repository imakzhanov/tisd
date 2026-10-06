#!/bin/bash

SCRIPT_DIR="./func_tests/scripts"
DATA_DIR="./func_tests/data"


GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# тест пункта 10 меню перезаписывает report.txt, поэтому сохраняем его
cp report.txt /tmp/report_saved.txt

POS_PASSED=0
POS_TOTAL=0
NEG_PASSED=0
NEG_TOTAL=0

for pos_in in "$DATA_DIR"/pos_*_in.txt; do
    echo -n "POS TEST $(basename "$pos_in") "

    test_num=$(basename "$pos_in" | sed 's/pos_\(.*\)_in.txt/\1/')
    pos_out="$DATA_DIR/pos_${test_num}_out.txt"
    args_file="$DATA_DIR/pos_${test_num}_args.txt"

    POS_TOTAL=$((POS_TOTAL + 1))


    if "$SCRIPT_DIR/pos_case.sh" "$pos_in" "$pos_out" "$args_file"; then
        POS_PASSED=$((POS_PASSED + 1))
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
    fi

done

for neg_in in "$DATA_DIR"/neg_*_in.txt; do

    echo -n "NEG TEST $(basename "$neg_in") "

    test_num=$(basename "$neg_in" | sed 's/neg_\(.*\)_in.txt/\1/')
    neg_out="$DATA_DIR/neg_${test_num}_out.txt"
    args_file="$DATA_DIR/neg_${test_num}_args.txt"

    NEG_TOTAL=$((NEG_TOTAL + 1))

    if "$SCRIPT_DIR/neg_case.sh" "$neg_in" "$neg_out" "$args_file"; then
        NEG_PASSED=$((NEG_PASSED + 1))
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
    fi
done

cp /tmp/report_saved.txt report.txt

# вывод сообщения
if [ $POS_PASSED -eq $POS_TOTAL ]; then
    echo -e "${GREEN}POSITIVE: $POS_PASSED/$POS_TOTAL"
else
    echo -e "${RED}POSITIVE: $POS_PASSED/$POS_TOTAL"
fi

if [ $NEG_PASSED -eq $NEG_TOTAL ]; then
    echo -e "${GREEN}NEGATIVE: $NEG_PASSED/$NEG_TOTAL"
else
    echo -e "${RED}NEGATIVE: $NEG_PASSED/$NEG_TOTAL"
fi


failed=$(( (POS_TOTAL-POS_PASSED) + (NEG_TOTAL-NEG_PASSED) ))

exit $failed
