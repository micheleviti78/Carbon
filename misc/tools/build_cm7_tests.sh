#!/bin/bash

TOP="$(cd $(dirname $0) && pwd)/../.."

BLD="$TOP/builds/tests"

TARGET=all
MAKE_CMD=make
MAKE_GEN="Unix Makefiles"

TESTS="\
    USB_HOST \
    ADS1248 \
    MW_EEPROM \
    EVENT_BUS \
    EXT_FLASH \
    FS \
    LOG_FILE_WRITER \
    LOG_FILE_WRITER_PERF \
    MPACK_READ_WRITE \
"

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

function build_test {
    (mkdir -p "$BLD/CM7_ETH_MWI_V0_$1" \
        && cd "$BLD/CM7_ETH_MWI_V0_$1" \
        && cmake $TOP/app/CM7 -G "$MAKE_GEN" -DMWI_TEST_BUILD="$1" \
        -DMWI_TARGET_BOARD=ETH_MWI_V0 -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DHAL_FULL_ASSERT=ON \
        -DFREERTOS_USE_ASSERT=ON -DDEBUG_LWIP=ON -DMWI_USE_ASSERT=ON \
        -DMWI_USE_TRACE=ON \
        && $MAKE_CMD -j8 $TARGET) || exit 1
}

for t in $TESTS; do
    build_test $t
done
