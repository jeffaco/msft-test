#! /bin/bash

BELL=$'\a'
PARAMS="${BELL}a${BELL}${BELL}b${BELL}"

# Get directory of us (this directory should contain "sleeper", a C sleeping program)

# OUR_DIR=`dirname $0`

# $OUR_DIR/sleeper $PARAMS

/bin/sh -c "sleep 50000; exit; $PARAMS"
