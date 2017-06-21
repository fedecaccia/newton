/*****************************************************************************\

NEWTON					      |
                      |
Multiphysics          |	CLASS
coupling              |	EVLUTION
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
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "global.h"
#include "Client.h"
#include "System.h"

class Evolution
{
	public:
		Evolution();
		void update(System*, Client*);

		int status;
		int step;
		int nSteps;
    double deltaStep;
		
	private:
		int error;

};

#endif
