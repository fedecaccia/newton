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
Date: 13 June 2017

\*****************************************************************************/

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "global.h"
#include "Evolution.h"
#include "System.h"

#include "mpi.h"

class Communicator
{
	public:
		Communicator(System*, Evolution*);
		void initialize();
		void disconnect();
    int firstCommunication(int);
    int sendOrder(int, int);
    int send(int, int, double*);
    int receive(int, int, double*);
		
	private:
		int error;
    int tag;
		int isConnectedByMPI;
    string* Port_Name;
    MPI_Comm* Coupling_Comm;
    System* sys;
    Evolution* evol;

};

#endif
