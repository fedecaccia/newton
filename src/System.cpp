/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	SYSTEM
calculations			|
						|

-------------------------------------------------------------------------------

System contains all the usefull information of the strucure of the problem, like number of unknowns, connectivities, etc.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "System.h"

using namespace::std;

/* System constructor
*/
System::System()
{
	error = NEWTON_SUCCESS;
}

/* System::construct
Sets the configuration of the system.

input: -
output: -

*/
void System::construct()
{
	checkError(error,"Error setting system structure.");
}