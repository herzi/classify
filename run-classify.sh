#!/bin/bash

export CLASSIFY_MODULES="`pwd -P`/.libs:${CLASSIFY_MODULES}"

if test $# -gt 0; then
        $@
else
        ./classify
fi

