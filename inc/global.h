/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	HEADER
in nonlinear			|	GLOBAL
calculations			|
						|

-------------------------------------------------------------------------------

Global sets global defines, variables and functions commonly used.

Author: Federico A. Caccia
Date: 3 June 2017

\*****************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <string>

// Errors
#define NEWTON_SUCCESS 0
#define NEWTON_ERROR 1

// MPI
#define NEWTON_ROOT 0

extern int world_size;
extern int irank;

//void functionTrying(void (*f)());
void checkError(int, std::string);
void rootPrints(std::string);

#endif