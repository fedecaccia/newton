/*****************************************************************************\

NEWTON					      |
                      |
Multiphysics          |	CLASS
coupling              |	PARSER
maste code            |
                      |

-------------------------------------------------------------------------------

Parser reads the input newton file and load all necessary data to solve the 
problem.

Date: 8 June 2017

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

#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "Communicator.h"
#include "Client.h"
#include "Evolution.h"
#include "Mapper.h"
#include "Solver.h"
#include "System.h"
#include "Debugger.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

class Parser
{
	public:
		Parser();
		void parseInput( System*, Evolution*, Solver*, Client*, Mapper*, Communicator*, Debugger*);
		void checkConsistency(System*);
    
    Debugger debug;

	private:
		bool isAComment(std:: string);
		std::string takeNextWord();
		std::string takeNextName();
    bool wordIsCard(std::string, std::string);
	  bool wordIsForbidden(std::string);

	  void checkImportantCards(System*, Solver*);
	  void checkClientProperties(System*, int);
    void checkConsistencyInPhases(System*);

		std::string loadClientAndTakeWord(System*);
		std::string loadCalcsAndTakeWord(System*);
		std::string loadGuessesAndTakeWord(System*);

		int error;
    std::string delim;
	  bool unkSaved;
    int globalRes;
		std::string word;
		std::ifstream configFile;
		int clientReaded;
    int fermiReaded;
    int relapReaded;
		int betaLoaded;
		int gammaLoaded;

};

#endif
