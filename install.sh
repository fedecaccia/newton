#!/bin/bash

## Check environmental variables
if [ -z ${MPI_DIR+x} ]; then 
    echo "MPI_DIR is unset";    
else 
    echo "MPI_DIR is set to '${MPI_DIR}'";
    
    if [ -z ${PETSC_DIR+x} ]; then 
        echo "PETSC_DIR is unset"; 
    else 
        echo "PETSC_DIR is set to '${PETSC_DIR}'";
        
        if [ -z ${NEWTON_DIR+x} ]; then 
            echo "NEWTON_DIR is unset, setting...";
            export NEWTON_DIR=$(pwd)
        else 
            echo "NEWTON_DIR is set to '${NEWTON_DIR}'";
        fi
      
        ## Build folders
        if ! ls bin 1> /dev/null 2>&1; then
            mkdir bin  
        fi
        if ! ls obj 1> /dev/null 2>&1; then
            mkdir obj
        fi
        if ! ls usr/obj 1> /dev/null 2>&1; then
            mkdir usr/obj
        fi
        if ! ls usr/bin 1> /dev/null 2>&1; then
            mkdir usr/bin
        fi
        make all
    fi
fi
