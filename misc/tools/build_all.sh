#!/bin/bash

TOP="$(cd $(dirname $0) && pwd)/../.."

BLD="$TOP/builds"

TARGET=all
MAKE_CMD=make
MAKE_GEN="Unix Makefiles"

if [ $# -ge 1 ]; then
    TARGET=$1
    if [ $# -ge 2 ]; then
        if [ $2 == "ninja" ]; then
            MAKE_CMD="ninja"
            MAKE_GEN="Ninja"
        elif [ $2 != "make" ]; then
            echo "unknown build generator"
            exit 1
        fi
    fi
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

(mkdir -p "$BLD/CM4_CARBON" \
    && cd "$BLD/CM4_CARBON" \
    && cmake ../../CM4 \
    && $MAKE_CMD -j8 $TARGET) || exit 1
    
(mkdir -p "$BLD/CM7_CARBON" \
    && cd "$BLD/CM7_CARBON" \
    && cmake ../../CM7 \
    && $MAKE_CMD -j8 $TARGET) || exit 1
    
(mkdir -p "$BLD/CM7_CARBON_TRACE" \
    && cd "$BLD/CM7_CARBON_TRACE" \
    && cmake ../../CM7 -DFREERTOS_USE_TRACE=ON \
    && $MAKE_CMD -j8 $TARGET) || exit 1
    
(mkdir -p "$BLD/CM4_CARBON_TRACE" \
    && cd "$BLD/CM4_CARBON_TRACE" \
    && cmake ../../CM4 -DFREERTOS_USE_TRACE=ON \
    && $MAKE_CMD -j8 $TARGET) || exit 1
    
(mkdir -p "$BLD/CM7_CARBON_SD_TEST" \
    && cd "$BLD/CM7_CARBON_SD_TEST" \
    && cmake ../../CM7 -DSD_TEST=ON \
    && $MAKE_CMD -j8 $TARGET) || exit 1
