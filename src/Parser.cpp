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

#include "Parser.h"

using namespace::std;

/* Parser constructor
*/
Parser::Parser()
{
	error = NEWTON_SUCCESS;
}

/* Parser::parseInput
Parse the input file.

input: System pointer
output: -

*/
void Parser::parseInput(System* sys, Evolution* evol)
{
	rootPrints("Parsing configuration file...");

	// Count amount of client codes in order to allocate
	configFile.open("newton.config");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){
			if(aux=="CODE_SPECIFIC"){
        sys->nCodes++;
				//configFile >> aux;
				//stringstream(aux) >> evol->nSteps;
			}
			// Next word
			configFile >> aux;
		}
	}
	else{
		error = NEWTON_ERROR;
		checkError(error,"Error opening \"newton.config\"");
	}
  configFile.close();
  
  // Allocations
  



	// Complete parsing
	configFile.open("newton.config");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){
			if(aux=="N_STEPS"){
				configFile >> aux;
				stringstream(aux) >> evol->nSteps;
			}
			// Next word
			configFile >> aux;
		}
	}
	else{
		error = NEWTON_ERROR;
		checkError(error,"Error opening \"newton.config\"");
	}
  configFile.close();
}

/* Parser::checkConsistency
Check if the data readed is consistent.

input: -
output: -

*/
void Parser::checkConsistency()
{
	checkError(error,"Error checking consistenc in config data");
}
