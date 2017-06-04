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

class Parser
{
	public:
		Parser();
		void parseInput();		
		
	private:
		int error;

};

#endif