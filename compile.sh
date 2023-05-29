#!/usr/bin/env bash

set -e
set -o pipefail
set -u

CC="gcc"
COMPILER="ttc.c"

function compile {
    $CC $COMPILER -o ttc
    ./ttc $1 | $CC -x c -o ttc.out -
    ./ttc.out
}

compile $1
