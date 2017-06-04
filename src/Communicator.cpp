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
	error = NEWTON_SUCCESS;
}

/* Communicator::initialize
Starts the communication.

input: -
output: -

*/
void Communicator::initialize()
{
	checkError(error,"Error starting communication.");
}