/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | EVOLUTION
maste code            |
                      |

-------------------------------------------------------------------------------

Evolution updates the evolution parameter and other problem dependent variables
and configurations.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "Evolution.h"

using namespace::std;

/* Evolution constructor
*/
Evolution::Evolution()
{
	// Actual evolution step
	step = 0;
  // Total steps
  nSteps = 1;
  // Delta step
  deltaStep = 1;
  // Run status
	status = NEWTON_INCOMPLETE;
  // Initial state
	error = NEWTON_SUCCESS;
}

/* Evolution::update
Updates evolution parameters.

input: -
output: -

*/
void Evolution::update(System* sys, Client* client)
{
	step++;
  client->updateVars(deltaStep);
	if(step==nSteps){
		status = NEWTON_COMPLETE;
	}
  else{
    // Update file names and command args in I/O case
    sys->setFilesAndCommands(step);
  }
	checkError(error,"Error updating evolution.");
}
