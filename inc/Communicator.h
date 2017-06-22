/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | COMMUNICATOR
maste code            |
                      |

-------------------------------------------------------------------------------

Stablish mpi communitaction with clients.

Date: 14 June 2017

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

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "global.h"
#include "Evolution.h"
#include "System.h"
#include "Debugger.h"

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
    
    Debugger debug;
		
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
