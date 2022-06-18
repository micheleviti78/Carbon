#!/bin/bash

TOP="$(cd $(dirname $0) && pwd)/../.."

DIRS="CM7 CM4 common"

for pat in clang-format{-10,}; do
    FMT=`which $pat`
    if [ $? == 0 ]; then
        break
    fi
done

if [ -z "$FMT" ]; then
    echo "failed to find clang-format"
    exit 1
fi

for path in "$@"; do
    if [ ! -d "${path}" ]; then
        echo "${path} is not a directory"
        exit 1
    fi
done

pushd "$TOP" &>/dev/null
for path in $DIRS; do
    find "$path" \(             \
        -name '*.c'             \
        -o -name '*.cpp'        \
        -o -name '*.h'          \
        -o -name '*.hpp'        \
        \) | xargs -I{} "${FMT}" -n '{}'
done
popd &>/dev/null
