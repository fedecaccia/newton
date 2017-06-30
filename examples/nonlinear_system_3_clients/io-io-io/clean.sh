#!/bin/bash

if ls *.out 1> /dev/null 2>&1; then
    rm *.out
fi
if ls *.log 1> /dev/null 2>&1; then
    rm *.log
fi
if ls *step* 1> /dev/null 2>&1; then
    rm *step*
fi
if ls mf 1> /dev/null 2>&1; then
    rm mf
fi
if ls ompiuri 1> /dev/null 2>&1; then
    rm ompiuri
fi
