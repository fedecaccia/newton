/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	EVOLUTION
calculations			|
						|

-------------------------------------------------------------------------------

Evolution updates the evolution parameter and other problem dependent variables and configurations.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "Evolution.h"

using namespace::std;

/* Evolution constructor
*/
Evolution::Evolution()
{
	error = NEWTON_SUCCESS;
}

/* Evolution::start
Starts the evolution.

input: -
output: -

*/
void Evolution::start()
{
	checkError(error,"Error starting evolution.");
}