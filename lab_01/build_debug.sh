#!/bin/bash

PROJECT_ROOT="$(dirname "$(realpath "$0")")"

gcc -g -O0 \
    -std=c99 \
    -Wall -Werror -Wpedantic -Wextra -Wvla \
    -I"$PROJECT_ROOT" \
    -o "$PROJECT_ROOT/app.exe" \
    $PROJECT_ROOT/*.c \
    -lm
