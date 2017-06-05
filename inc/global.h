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
#define NEWTON_SUCCESS 				  0
#define NEWTON_ERROR 				    1

// MPI
#define NEWTON_ROOT 				    0

// Communication with clients
#define NEWTON_CONNECTED			  0
#define NEWTON_DISCONNECTED		  1

// Running status
#define NEWTON_COMPLETE 			  0
#define NEWTON_INCOMPLETE			  1

// Numerial method in non linear iteration
#define D2N_SERIAL              0
#define D2N_PARALLEL            1
#define NEWTON                  2
#define SECANT                  3
#define BROYDEN                 4

extern int world_size;
extern int irank;

//void functionTrying(void (*f)());
void checkError(int, std::string);
void rootPrints(std::string);
std::string int2str(int);
std::string dou2str(double);

#endif
