#!/bin/bash

TOP="$(cd $(dirname $0) && pwd)/../.."

if [ $# -lt 2 ]; then
    if [ ! -f "$TOP/builds/sim/compile_commands.json" ]; then
        echo "Usage: $0 <compile_commands.json> <srcs...>"
        exit 1
    else
        CCMD="$TOP/builds/sim/compile_commands.json"
        if [ $# -ge 1 ]; then
            DIRS=$*
        else
            DIRS="$TOP/app/common $TOP/app/sim"
        fi
    fi
else
    CCMD=$1
    shift
    DIRS=$*
fi

for pat in clang-tidy{-10,}; do
    TDY=`which $pat`
    if [ $? == 0 ]; then
        break
    fi
done

if [ -z "$TDY" ]; then
    echo "failed to find clang-tidy"
    exit 1
fi

for path in $DIRS; do
    if [ ! -d "${path}" ]; then
        echo "${path} is not a directory"
        exit 1
    fi
done

#pushd "$TOP" &>/dev/null
for path in $DIRS; do
    find "${path}" \(           \
        -name '*.cpp'           \
        -o -name '*.hpp'        \
        \) | xargs -I{} "${TDY}" -p "$CCMD" '{}'
done
#popd &>/dev/null
