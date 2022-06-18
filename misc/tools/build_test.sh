#!/bin/bash

TOP="$(cd $(dirname $0) && pwd)/../.."

BLD="$TOP/builds"

if [ $# -ge 1 ]; then
    TARGET=$1
else
    TARGET=all
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

(mkdir -p "$BLD/CM4_ETH_MWI_V0_TEST_SPI" \
    && cd "$BLD/CM4_ETH_MWI_V0_TEST_SPI" \
    && cmake ../../app/CM4 -DMWI_TARGET_BOARD=ETH_MWI_V0 -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DHAL_FULL_ASSERT=ON \
    -DFREERTOS_USE_ASSERT=ON -DMWI_USE_ASSERT=ON -DMWI_USE_TRACE=ON -DMWI_TEST_SPI=ON\
    && make -j8 $TARGET) || exit 1

(mkdir -p "$BLD/CM4_ETH_MWI_V0_TEST_FPGA_COMMAND" \
    && cd "$BLD/CM4_ETH_MWI_V0_TEST_FPGA_COMMAND" \
    && cmake ../../app/CM4 -DMWI_TARGET_BOARD=ETH_MWI_V0 -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DHAL_FULL_ASSERT=ON \
    -DFREERTOS_USE_ASSERT=ON -DMWI_USE_ASSERT=ON -DMWI_USE_TRACE=ON -DMWI_TEST_FPGA_COMMAND=ON\
    && make -j8 $TARGET) || exit 1

(mkdir -p "$BLD/CM4_ETH_MWI_V0_TEST_FPGA_DATA" \
    && cd "$BLD/CM4_ETH_MWI_V0_TEST_FPGA_DATA" \
    && cmake ../../app/CM4 -DMWI_TARGET_BOARD=ETH_MWI_V0 -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DHAL_FULL_ASSERT=ON \
    -DFREERTOS_USE_ASSERT=ON -DMWI_USE_ASSERT=ON -DMWI_USE_TRACE=ON -DMWI_TEST_FPGA_DATA=ON\
    && make -j8 $TARGET) || exit 1
