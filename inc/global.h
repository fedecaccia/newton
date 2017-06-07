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
#define NEWTON_SUCCESS 				      0
#define NEWTON_ERROR 				        1
    
// MPI variables
#define NEWTON_ROOT 				        0
    
// MPI communication stablished
#define NEWTON_CONNECTED			      0
#define NEWTON_DISCONNECTED		      1

// Communication way with client   
#define NEWTON_UNKNOWN_CONNECTION   0
#define NEWTON_MPI_COMMUNICATION    1
#define NEWTON_SPAWN                2

// Mapper
#define NEWTON_NO_MAP               0
#define NEWTON_PRE_MAP              1
#define NEWTON_POST_MAP             2
#define NEWTON_BOTH_MAP             3

// Stages in residual calculation
#define NEWTON_PRE_SEND             0
#define NEWTON_POST_RECV            1


// Running status   
#define NEWTON_COMPLETE 			      0
#define NEWTON_INCOMPLETE			      1
    
// Numerial method in non linear iteration
#define EXPLICIT_SERIAL             0
#define EXPLICIT_PARALLEL           1
#define NEWTON                      2
#define SECANT                      3
#define BROYDEN                     4

// Client code
#define RELAP                       0
#define FERMI                       1
#define USER_CODE                   2

extern int world_size;
extern int irank;

//void functionTrying(void (*f)());
void checkError(int, std::string);
void rootPrints(std::string);
std::string int2str(int);
std::string dou2str(double);

#endif
