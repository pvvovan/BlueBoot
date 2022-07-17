#!/bin/bash

SCRIPT_PATH=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT_PATH`
export PICO_SDK_PATH=${SCRIPT_DIR}/pico-sdk/

rm -rf ${SCRIPT_DIR}/moverbuild
mkdir ${SCRIPT_DIR}/moverbuild
cd ${SCRIPT_DIR}/moverbuild
cmake ${SCRIPT_DIR} -D CMAKE_BUILD_TYPE=Debug
make all
