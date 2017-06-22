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

Date: 4 June 2017

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
