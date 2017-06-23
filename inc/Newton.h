/*****************************************************************************\

NEWTON					      |
                      |
Multiphysics          |	CLASS
coupling              |	NEWTON
maste code            |
                      |

-------------------------------------------------------------------------------

Newton manages the three main stages in residual calculation: initializing 
everything to start calculations, administrates evolution parameters, controls 
that solution to residual equations converge in each evolution step, and ends
everything safety.

Date: 3 June 2017

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

#ifndef NEWTON_H
#define NEWTON_H

#include "global.h"
#include "Parser.h"
#include "Mapper.h"
#include "System.h"
#include "Evolution.h"
#include "Communicator.h"
#include "Solver.h"
#include "Debugger.h"

#include "mpi.h"
#include "petsc.h"
#include <time.h>
#include <string>

class Newton
{
	public:
		Newton();
		void initialize();
		void configureDebugger();
		void run();
		void finish();

		Parser* NewtonParser;
		System* NewtonSystem;
		Evolution* NewtonEvolution;
		Communicator* NewtonComm;
		Solver* NewtonSolver;
    Mapper* NewtonMap;    
    Debugger debug;
		
	private:
		int error;
    double firstClick, lastClick;
    double click1, click2;
    

};

#endif
