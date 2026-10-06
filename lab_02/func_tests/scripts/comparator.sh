#!/bin/bash

if [ $# -ne 2 ]; then
    exit 2
fi

if [ ! -f "$1" ] || [ ! -f "$2" ]; then
    exit 2
fi

if cmp "$1" "$2" ; then
    exit 0
fi

exit 1
