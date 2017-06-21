/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | COMMUNICATOR
maste code            |
                      |

-------------------------------------------------------------------------------

Stablish mpi communitaction with clients.

Author: Federico A. Caccia
Date: 14 June 2017

\*****************************************************************************/

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "global.h"
#include "Evolution.h"
#include "System.h"

#include "mpi.h"
#include <string.h>

class Communicator
{
	public:
		Communicator(System*, Evolution*);
		void initialize();
		void disconnect();
    int firstCommunication();
    int sendOrder(int);
    int send(int, int, double*);
    int receive(int, int, double*);
		
	private:
		int error;
    int tag;
		int isConnectedByMPI;
    std::string* Port_Name;
    MPI_Comm* Coupling_Comm;
    System* sys;
    Evolution* evol;

};

#endif
