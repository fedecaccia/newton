/*****************************************************************************\

NEWTON					      |
                      |
Multiphysics          |	GLOBAL
coupling              |	HEADER
maste code            |
                      |

-------------------------------------------------------------------------------

Global sets global defines, variables and functions commonly used.

Date: 3 June 2017

-------------------------------------------------------------------------------

Copyright (C) 2017 Federico A. Caccia

This file is part of Newton.

Newton is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Newton is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Newton.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

// Errors
#define NEWTON_SUCCESS 				      0
#define NEWTON_ERROR 				        1
    
// MPI variables
#define NEWTON_ROOT 				        0
    
// MPI communication stablished
#define NEWTON_CONNECTED			      0
#define NEWTON_DISCONNECTED		      1

// MPI communication order
#define NEWTON_CONTINUE		          0
#define NEWTON_RESTART      	      1
#define NEWTON_FINISH       	      2
#define NEWTON_ABORT        	      3

// Communication way with client   
#define NEWTON_UNKNOWN_CONNECTION   0
#define NEWTON_SPAWN                1
#define NEWTON_SYSTEM               2
#define NEWTON_MPI_PORT             3
#define NEWTON_MPI_COMM             4

// Communicator color
#define NEWTON_COLOR								0

// Mapper in each stage
#define NEWTON_NO_MAP               0
#define NEWTON_MAP                  1

// Running status   
#define NEWTON_COMPLETE 			      0
#define NEWTON_INCOMPLETE			      1
    
// Numerial method in non linear iteration
#define NO_METHOD                  -1
#define EXPLICIT_SERIAL             0
#define EXPLICIT_PARALLEL           1
#define NEWTON                      2
#define SECANT                      3
#define BROYDEN                     4

// Client code type
#define TEST                        0
#define USER_CODE                   1
#define RELAP_POW2TH                2
#define FERMI_XS2POW                3
#define NEUTRONIC_CR2KP             4
#define NEUTRONIC_KP2CR             5
#define PUMA_TH2POW                 6

// Debugger
#define MAX_LOG                    10
#define GLOBAL_LOG                  0
#define ITER_LOG                    1
#define RES_LOG                     2
#define X_LOG                       3
#define J_LOG                       4
#define UNK_LOG                     5

extern int global_size;
extern int global_rank;
extern int local_rank;
extern int local_size;
extern MPI_Comm newton_comm;


//void functionTrying(void (*f)());
void checkError(int, std::string);
void rootPrints(std::string);
std::string int2str(int);
std::string dou2str(double);

#endif
