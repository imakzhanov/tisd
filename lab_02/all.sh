#!/bin/bash

cd "$(dirname "$0")" || exit 1
SCRIPT_DIR="./func_tests/scripts"

TOTAL=0
PASSED=0
FAILED=0

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'


run_test() {
    local name="$1"
    local build_script="$2"

    echo "=============================="
    echo "TEST: $name"

    echo -n "BUILD "
    if "./$build_script" 2> /dev/null; then
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
        TOTAL=$((TOTAL + 1))
        FAILED=$((FAILED + 1))

        "./clean.sh"
        return 1
    fi

    # Запуск тестов
    echo "Starting tests: "
    if "$SCRIPT_DIR/func_tests.sh"; then
        echo -e "${GREEN}TESTS PASSED${NC}"
        TOTAL=$((TOTAL + 1))
        PASSED=$((PASSED + 1))

        "./clean.sh"
        return 0
    else
        echo -e "${RED}TESTS FAILED${NC}"
        TOTAL=$((TOTAL + 1))
        FAILED=$((FAILED + 1))

        "./clean.sh"
        return 1
    fi
}

echo -e "${GREEN}---------- $(basename "$PWD") ----------${NC}"

# запуск сборок
run_test "Debug" "build_debug.sh"
run_test "Release" "build_release.sh"

"./clean.sh"

if [ $TOTAL -gt 0 ]; then
    PERCENT=$((PASSED * 100 / TOTAL))
else
    PERCENT=0
fi

echo -e "${GREEN}---------------------------------\n"
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}Success: $PERCENT%${NC}"
    exit 0
else
    echo -e "${RED}Success: $PERCENT%${NC}"
    exit $FAILED
fi
