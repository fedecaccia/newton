/*****************************************************************************\

NEWTON                 |
                       |
Implicit coupling      |    CLASS
in nonlinear           |    PARSER
calculations           |
                       |

-------------------------------------------------------------------------------

Parser reads the input newton file and load all necessary data to solve the 
problem.

Author: Federico A. Caccia
Date: 8 June 2017

\*****************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "Evolution.h"
#include "Solver.h"
#include "System.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

class Parser
{
	public:
		Parser();
		void parseInput(System*, Evolution*, Solver*);
		void checkConsistency(System*);
		
	private:
		bool isAComment(std:: string);

		int error;
    	int codeReaded;
		std::string word;
		std::ifstream configFile;

};

#endif
