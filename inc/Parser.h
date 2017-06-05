/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	PARSER
calculations			|
						|

-------------------------------------------------------------------------------

Parser read the input newton file and load all the necessary data to solve the problem.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "System.h"
#include "Evolution.h"

#include <iostream> //endl, cout
#include <fstream> // ifsteam
#include <sstream> // stingstream
#include <string>

class Parser
{
	public:
		Parser();
		void parseInput(System*, Evolution*);
		void checkConsistency();
		
	private:
		int error;
		std::string aux;
		std::ifstream configFile;

};

#endif