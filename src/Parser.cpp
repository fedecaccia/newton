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

input: -
output: -

*/
void Parser::parseInput()
{
	checkError(error,"Error opening input file.");
}