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
void Parser::parseInput(System* sys, Evolution* evol, Solver* sol)
{
	rootPrints("Parsing configuration file...");

	// Count necessary elements in order to allocate
  
	configFile.open("newton.config");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){
			
      if(aux=="CODE_SPECIFIC"){
        sys->nCodes++;
			}
      
      if(aux=="PHASE"){
        sys->nPhasesPerIter++;
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
  
  sys->allocate();  
  
	// Complete parsing
  
	configFile.open("newton.config");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){
      
      
      // Parsing numerical methods
      
      if(aux=="N_STEPS"){
				configFile >> aux;
				stringstream(aux) >> evol->nSteps;
			}
      
      if(aux=="DELTA_STEP"){
				configFile >> aux;
				stringstream(aux) >> evol->deltaStep;
			}
      
      if(aux=="NONLINEAR_ABS_TOLERANCE"){
				configFile >> aux;
				stringstream(aux) >> sol->nltol;
			}
      
      if(aux=="NONLINEAR_MAX_ITERATIONS"){
				configFile >> aux;
				stringstream(aux) >> sol->maxIter;
			}
      
      if(aux=="METHOD"){
				configFile >> aux;
				stringstream(aux) >> sol->method;
			}
      
      if(aux=="DX_IN_JAC_CALC"){
				configFile >> aux;
				stringstream(aux) >> sol->dxJacCalc;
			}
      
      if(aux=="FREC_IN_JAC_CALC"){
				configFile >> aux;
				stringstream(aux) >> sol->fJacCalc;
			}
      
			if(aux=="EXPLICIT_SERIAL_ORDER"){                
        for (int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          if(aux=="PHASE"){
            configFile >> aux;
            while(aux!="PHASE"){
              sys->nCodesInPhase[iPhase]++;
              configFile >> aux;
            }
            sys->codeToConnectInPhase[iPhase] = new int[sys->nCodesInPhase[iPhase]];
          }
          else{
            error = NEWTON_ERROR;
            checkError(error, "Bad input. Check card EXPLICIT_SERIAL_ORDER.");
          }
        }
      }
			
      // Parsing code specific
      
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
