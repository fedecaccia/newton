/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	PARSER
calculations			    |
                      |

-------------------------------------------------------------------------------

Parser reads the input newton file and load all necessary data to solve the
problem.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "Evolution.h"
#include "Solver.h"
#include "System.h"

#include <iostream> //endl, cout
#include <fstream> // ifsteam
#include <sstream> // stingstream
#include <string>

class Parser
{
	public:
		Parser();
		void parseInput(System*, Evolution*, Solver*);
		void parseInputOld(System*, Evolution*, Solver*);
		void checkConsistency(System*);
		
	private:
    bool endCard(string myStr);
    bool startModule(string myStr);
    bool endModule(string myStr);
  
		int error;
    int jCode;
		std::string aux;
		std::ifstream configFile;

};

#endif
