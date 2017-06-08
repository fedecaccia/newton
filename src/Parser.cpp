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
Date: 8 June 2017

\*****************************************************************************/

#include "Parser.h"

using namespace::std;

/* Parser constructor
*/
Parser::Parser()
{   
  // Initial error value
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
  
	// Count necessary things in first stage
    
	configFile.open("newton.configtest");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){

      // Count amount of codes in order to allocate
      if(aux=="CODE_SPECIFIC"){
        sys->nCodes++;
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


  
  exit(1);

}







/* Parser::endCard
Identify last input in the card

input: String
output: true if it's the end, false in other case

*/
bool Parser::endCard(string myStr)
{
  if (!myStr.empty())
  {
    char lastChar = *myStr.rbegin();
    if(lastChar==';'){
      return true;
    }
  }
  return false;  
}

/* Parser::startModule
Identify start of the module card

input: String
output: true if it's the end, false in other case

*/
bool Parser::startModule(string myStr)
{
  if (!myStr.empty())
  {
    char lastChar = *myStr.rbegin();
    if(lastChar=='{'){
      return true;
    }
    char firstChar = *myStr.begin();
    if(firstChar=='{'){
      return true;
    }
  }
  return false;  
}

/* Parser::endModule
Identify end of the module card

input: String
output: true if it's the end, false in other case

*/
bool Parser::endModule(string myStr)
{
  if (!myStr.empty())
  {
    char lastChar = *myStr.rbegin();
    if(lastChar=='}'){
      return true;
    }
    char firstChar = *myStr.begin();
    if(firstChar=='}'){
      return true;
    }
  }
  return false;  
}





/* Parser::parseInputOld
Parse the input file.

input: System pointer
output: -

*/
void Parser::parseInputOld(System* sys, Evolution* evol, Solver* sol)
{
	rootPrints("Parsing configuration file...");
  
  // Number of client codes readed
  jCode = 0;

	// Count necessary things in first stage
    
	configFile.open("newton.config");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){

      // Count amount of codes in order to allocate
      if(aux=="CODE_SPECIFIC"){
        sys->nCodes++;
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
  
  // Allocates
  
  sys->allocate1();   
  
	// Count necessary things in second stage
  
	configFile.open("newton.config");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){
      
      // Count amount of phases and codes in each phase in explicit 
      // serial method
      if(aux=="EXPLICIT_SERIAL_ORDER"){
        int nCodesInPhases = 0;        
        while(nCodesInPhases<sys->nCodes && aux!="END_EXPLICIT"){
          configFile >> aux;
          while(aux!="&" && aux!="END_EXPLICIT"){
            nCodesInPhases++;
            configFile >> aux;
          }
          sys->nPhasesPerIter++;
        }        
      }
      
      // Count amount of arguments that each binary client code takes
      if(aux=="CODE_SPECIFIC"){
        configFile >> aux;
        while(aux!="END_CODE"){
        
          if(aux=="N_ARGS"){
            configFile >> aux;
            stringstream(aux) >> sys->code[jCode].nArgs;
          }
          configFile >> aux;
        }        
        jCode++;
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
  
  // Alloocate things  
  sys->allocate2();
  // Reinitialize count
  jCode = 0;
  
	// Count necessary things in third stage

	configFile.open("newton.config");
	if (configFile.is_open()){
		configFile >> aux;
		while(!configFile.eof()){
      
      // Count amount of codes in each phase in explicit serial method
      
      if(aux=="EXPLICIT_SERIAL_ORDER"){
        int iPhase = 0;
        
        while(sys->nCodesInPhase[iPhase] <sys->nCodes && aux!="END_EXPLICIT"){
          configFile >> aux;
          while(aux!="&" && aux!="END_EXPLICIT"){
            sys->nCodesInPhase[iPhase]++;
            configFile >> aux;
          }          
          iPhase++;
        }
      
        // TEST
        //~ for(int i=0; i<sys->nPhasesPerIter; i++){
          //~ cout<<sys->nCodesInPhase[i]<<endl;
        //~ }
        //~ exit(1);
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
  
  
  // Allocate things
  sys->allocate3();
	
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
        configFile >> aux;
        for (int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
            stringstream(aux) >> sys->codeToConnectInPhase[iPhase][iCode];         
            configFile >> aux;
          }
          configFile >> aux;
        }
        
        // TEST
        //~ for (int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          //~ for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
            //~ cout<< sys->codeToConnectInPhase[iPhase][iCode]<<" ";
          //~ }
          //~ cout<<endl;
        //~ }
        //~ exit(1);        
      }
           
			
      // Parsing code specific
      
      if(aux=="CODE_SPECIFIC"){
				configFile >> aux;
        while(aux!="END_CODE"){
          
          if(aux=="CODE_ID"){
            configFile >> aux;
            stringstream(aux) >> sys->code[jCode].id;
          }          
          if(aux=="TYPE"){
            configFile >> aux;
            if(aux == "TEST" || aux == "test" || aux == "Test"){
              sys->code[jCode].type = TEST;
            }
            else if(aux == "RELAP_POW2TH" || aux == "relap_pow2th" || aux == "Relap_pow2th"){
              sys->code[jCode].type = RELAP_POW2TH;
            }
            else if(aux == "FERMI_XS2POW" || aux == "fermi_xs2pow" || aux == "Fermi_xs2pow"){
              sys->code[jCode].type = FERMI_XS2POW;
            }
            else if(aux == "USER_CODE" || aux == "user_code" || aux == "User_code"){
              sys->code[jCode].type = USER_CODE;
            }
            else{
              error = NEWTON_ERROR;
              checkError(error,("Error reading TYPE card for code: "+jCode));
            }
            // TEST
            //~ cout<<"Type: "<<sys->code[jCode].id<<endl;
            //~ exit(1);
          }
          if(aux=="CONNECTION"){
            configFile >> aux;
            if(aux == "MPI" || aux == "mpi" || aux == "Mpi"){
              sys->code[jCode].connection = NEWTON_MPI_COMMUNICATION;
            }
            else if(aux == "SPAWN" || aux == "spawn" || aux == "Spawn"){
              sys->code[jCode].connection = NEWTON_SPAWN;
            }
            else if(aux == "PPLEP" || aux == "pplep" || aux == "Pplep"){
              sys->code[jCode].connection = NEWTON_PPLEP;
            }
            else{
              sys->code[jCode].connection = NEWTON_UNKNOWN_CONNECTION;
              error = NEWTON_ERROR;
              checkError(error,("Error reading CONNECTION card for code: "+jCode));
            }
          }
          if(aux=="N_PROCS"){
            configFile >> aux;
            stringstream(aux) >> sys->code[jCode].nProcs;
          }          
          if(aux=="FROM_CODE_ID"){
            int iCode;
            configFile >> aux;
            stringstream(aux) >> iCode;
            // Dummy word VAR
            configFile >> aux;
            configFile >> aux;
            stringstream(aux) >> sys->code[jCode].nCalculationsFromCode[iCode];
          }
          if(aux=="TO_CODE_ID"){
            int iCode;
            configFile >> aux;
            stringstream(aux) >> iCode;
            // Dummy word VAR
            configFile >> aux;
            configFile >> aux;
            stringstream(aux) >> sys->code[jCode].nCalculations2Code[iCode];
          }
          if(aux=="GUESS_MAP"){
            configFile >> sys->code[jCode].map[NEWTON_PRE_SEND];
          }
          if(aux=="CALC_MAP"){
            configFile >> sys->code[jCode].map[NEWTON_POST_RECV];
          }
          // Strings
          if(aux=="INPUT_NAME"){
            configFile >> sys->code[jCode].inputModelName;
          }
          if(aux=="INPUT_EXT"){
            configFile >> sys->code[jCode].inputExt;
          }
          if(aux=="RESTART_NAME"){
            configFile >> sys->code[jCode].restartName;
          }
          if(aux=="RESTART_EXT"){
            configFile >> sys->code[jCode].restartExt;
          }
          if(aux=="RESTART_PATH"){
            configFile >> sys->code[jCode].restartPath;
          }
          if(aux=="OUTPUT_NAME"){
            configFile >> sys->code[jCode].outputName;
          }
          if(aux=="OUTPUT_EXT"){
            configFile >> sys->code[jCode].outputExt;
          }
          if(aux=="OUTPUT_PATH"){
            configFile >> sys->code[jCode].outputPath;
          }
          if(aux=="BIN_COMMAND"){
            configFile >> sys->code[jCode].binCommand;
          }
          if(aux=="ARGS"){
            for(int iArg=0; iArg<sys->code[jCode].nArgs; iArg++){
              configFile >> sys->code[jCode].arg[iArg];
            }
          }
          
          configFile >> aux;
        }
        jCode++;
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

input: System pointer
output: -

*/
void Parser::checkConsistency(System* sys)
{
  // Checking guesses and calculations
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    for(int jCode=0; jCode<sys->nCodes; jCode++){
      if(sys->code[iCode].nCalculations2Code[jCode]
      != sys->code[jCode].nCalculationsFromCode[iCode]){
        rootPrints("Incompatible guesses and calculations.Check: "+int2str(iCode)+" -> "+int2str(jCode));
      }
    }
  }
   
  
	checkError(error,"Error checking consistency in config data");
}
