#!/bin/bash

SCRIPT_PATH=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT_PATH`

export PICO_SDK_PATH=${SCRIPT_DIR}/pico-sdk/

rm -rf ${SCRIPT_DIR}/probebuild
mkdir ${SCRIPT_DIR}/probebuild
cd ${SCRIPT_DIR}/probebuild
cmake ${SCRIPT_DIR}/picoprobe
make
