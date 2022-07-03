#!/bin/bash

IN="src/main.cc"
OUT="catc"

FLAGS="-Wno-write-strings -g3 -Iinc"
LINK="-lstdc++"

RUN_ON_BUILD=0
[ -z $1 ] && RUN_ON_BUILD=1

clear
clang $IN -o $OUT $FLAGS $LINK
SUCCESS=$?

if [ $RUN_ON_BUILD = 0 ] && [ ! $SUCCESS -ne 0 ]; then
	./$OUT
fi

exit $SUCCESS