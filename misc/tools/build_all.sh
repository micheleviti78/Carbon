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
##
#(mkdir -p "$BLD/CM4_ETH_MWI_V0_dev" \
#    && cd "$BLD/CM4_ETH_MWI_V0_dev" \
#    && cmake ../../CM4 -G "$MAKE_GEN" \
#    -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
#    -DHAL_FULL_ASSERT=ON -DFREERTOS_USE_ASSERT=ON \
#    && $MAKE_CMD -j8 $TARGET) || exit 1
##
(mkdir -p "$BLD/CM7_CARBON" \
    && cd "$BLD/CM7_CARBON" \
    && cmake ../../CM7 \
    && $MAKE_CMD -j8 $TARGET) || exit 1

#(mkdir -p "$BLD/CM7_ETH_MWI_V0_dev" \
#    && cd "$BLD/CM7_ETH_MWI_V0_dev" \
#    && cmake ../../CM7 -G "$MAKE_GEN" \
#    -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
#    -DHAL_FULL_ASSERT=ON -DFREERTOS_USE_ASSERT=ON -DDEBUG_LWIP=ON \
#    && $MAKE_CMD -j8 $TARGET) || exit 1
