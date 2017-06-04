/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	MAPPER
calculations			|
						|

-------------------------------------------------------------------------------

Mapper manages the preprocessing and / or postprocessing of the variables received and / or sended to the clients.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "Mapper.h"

using namespace::std;

/* Evolution constructor
*/
Mapper::Mapper()
{
	error = NEWTON_SUCCESS;
}

/* Mapper::config
Set configuration parameters.

input: -
output: -

*/
void Mapper::config()
{
	checkError(error,"Error configuration mapper.");
}