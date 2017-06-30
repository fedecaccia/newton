#!/bin/bash

if ls examples/*/*/*.out 1> /dev/null 2>&1; then
    rm examples/*/*/*.out
fi
if ls examples/*/*/*.log 1> /dev/null 2>&1; then
    rm examples/*/*/*.log
fi
if ls examples/*/*/*step* 1> /dev/null 2>&1; then
    rm examples/*/*/*step*
fi
if ls examples/*/*/mf 1> /dev/null 2>&1; then
    rm examples/*/*/mf
fi
if ls examples/*/*/ompiuri 1> /dev/null 2>&1; then
    rm examples/*/*/ompiuri
fi
