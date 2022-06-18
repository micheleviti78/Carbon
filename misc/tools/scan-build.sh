#!/bin/bash

TOP="$(cd $(dirname $0) && pwd)/../.."

BLD="$TOP/builds"

for pat in scan-build{-10,}; do
    SBD=`which $pat`
    if [ $? == 0 ]; then
        break
    fi
done

if [ -z "$SBD" ]; then
    echo "failed to find scan-build"
    exit 1
fi

mkdir -p "$BLD/sim-scan-build"

(cd "$BLD/sim-scan-build" \
    && $SBD cmake ../../app/sim -DCMAKE_BUILD_TYPE=Debug -DDEBUG_LWIP=ON \
    && $SBD make -j8)
