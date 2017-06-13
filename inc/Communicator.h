/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	COMMUNICATOR
calculations			    |
                      |

-------------------------------------------------------------------------------

Stablish mpi communitaction with clients.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "global.h"
#include "System.h"

#include "mpi.h"

class Communicator
{
	public:
		Communicator();
		void initialize(System*);
		void disconnect();
		
	private:
		int error;
		int isConnectedByMPI;
    char** Port_Name;
    MPI_Comm* Coupling_Comm;

};

#endif
