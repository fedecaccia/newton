/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	COMMUNICATOR
calculations			|
						|

-------------------------------------------------------------------------------

Evolution updates the evolution parameter and other problem dependent variables and configurations.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "Communicator.h"

using namespace::std;

/* Evolution constructor
*/
Communicator::Communicator()
{
	// No connections at beginig
	isConnectedByMPI = NEWTON_DISCONNECTED;
	error = NEWTON_SUCCESS;
}

/* Communicator::initialize
Starts the communication.

input: -
output: -

*/
void Communicator::initialize()
{
  rootPrints("Checking communication with clients...");
	checkError(error,"Error starting communication.");
}

/* Communicator::disconnnect
Finishes the communication.

input: -
output: -

*/
void Communicator::disconnect()
{
	if(isConnectedByMPI==NEWTON_CONNECTED){
		rootPrints("Finishing client codes communications.");
		/*for(int iCode=0; iCode<Ld->nCodes;iCode++){
		    if (Ld->connectionType[iCode]==0){  // Tipo de conexión MPI
		      // tag de la recepción, el del código que lo recibe debe ser igual
		      tag = 100;
		      // Envío de órden: abortar (Están esperando recepción de orden al final de cada paso temporal)
		      ierror = MPI_Send (&order, 1, MPI_DOUBLE_PRECISION, 0, tag, (Comp_Comm[iCode]));
		      ierror = MPI_Comm_disconnect(&(Comp_Comm[iCode]));
		      ierror = MPI_Close_port(Port_Name[iCode]);
		    }
 		}*/
	}

	checkError(error,"Error starting communication.");
}
