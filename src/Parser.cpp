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
  
  // Number of client codes readed
  codeReaded = 0;

    // Count necessary things in first stage
    word = "";
    configFile.open("newton.configtest");
    if (configFile.is_open()){
      while(!configFile.eof()){

        // Take the rest of the line and erase it from lecture
        if(isAComment(word)){          
          getline(configFile, word);
          word = "";
        }

        // Count amount of codes in order to allocate
        if(word=="client"){
          sys->nCodes++;
        }
                
        // Next word
        configFile >> word;
        transform(word.begin(), word.end(), word.begin(), ::tolower);
      }
    }
    else{
        error = NEWTON_ERROR;
        checkError(error,"Error opening \"newton.config\"");
    }
  configFile.close();
  


  exit(1);



  // Allocates
  
  sys->allocate1();
  
    // Count necessary things in second stage
  
    configFile.open("newton.config");
    if (configFile.is_open()){
        configFile >> word;
        while(!configFile.eof()){
      
      // Count amount of phases and codes in each phase in explicit 
      // serial method
      if(word=="phases"){
        int nCodesInPhases = 0;        
        while(word!="&"){
          configFile >> word;
          while(word!="&" && word!="END_EXPLICIT"){
            nCodesInPhases++;
            configFile >> word;
          }
          sys->nPhasesPerIter++;
        }        
      }
      
      // Count amount of arguments that each binary client code takes
      if(word=="CODE_SPECIFIC"){
        configFile >> word;
        while(word!="END_CODE"){
        
          if(word=="N_ARGS"){
            configFile >> word;
            stringstream(word) >> sys->code[codeReaded].nArgs;
          }
          configFile >> word;
        }        
        codeReaded++;
            }
            
      // Next word
            configFile >> word;
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
  codeReaded = 0;
  
    // Count necessary things in third stage

    configFile.open("newton.config");
    if (configFile.is_open()){
        configFile >> word;
        while(!configFile.eof()){
      
      // Count amount of codes in each phase in explicit serial method
      
      if(word=="EXPLICIT_SERIAL_ORDER"){
        int iPhase = 0;
        
        while(sys->nCodesInPhase[iPhase] <sys->nCodes && word!="END_EXPLICIT"){
          configFile >> word;
          while(word!="&" && word!="END_EXPLICIT"){
            sys->nCodesInPhase[iPhase]++;
            configFile >> word;
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
            configFile >> word;
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
        configFile >> word;
        while(!configFile.eof()){
      
      
      // Parsing numerical methods
      
      if(word=="N_STEPS"){
                configFile >> word;
                stringstream(word) >> evol->nSteps;
            }
      
      if(word=="DELTA_STEP"){
                configFile >> word;
                stringstream(word) >> evol->deltaStep;
            }
      
      if(word=="NONLINEAR_ABS_TOLERANCE"){
                configFile >> word;
                stringstream(word) >> sol->nltol;
            }
      
      if(word=="NONLINEAR_MAX_ITERATIONS"){
                configFile >> word;
                stringstream(word) >> sol->maxIter;
            }
      
      if(word=="METHOD"){
                configFile >> word;
                stringstream(word) >> sol->method;
            }
      
      if(word=="DX_IN_JAC_CALC"){
                configFile >> word;
                stringstream(word) >> sol->dxJacCalc;
            }
      
      if(word=="FREC_IN_JAC_CALC"){
                configFile >> word;
                stringstream(word) >> sol->fJacCalc;
            }
      
            if(word=="EXPLICIT_SERIAL_ORDER"){
        configFile >> word;
        for (int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
            stringstream(word) >> sys->codeToConnectInPhase[iPhase][iCode];         
            configFile >> word;
          }
          configFile >> word;
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
      
      if(word=="CODE_SPECIFIC"){
                configFile >> word;
        while(word!="END_CODE"){
          
          if(word=="CODE_ID"){
            configFile >> word;
            stringstream(word) >> sys->code[codeReaded].id;
          }          
          if(word=="TYPE"){
            configFile >> word;
            if(word == "TEST" || word == "test" || word == "Test"){
              sys->code[codeReaded].type = TEST;
            }
            else if(word == "RELAP_POW2TH" || word == "relap_pow2th" || word == "Relap_pow2th"){
              sys->code[codeReaded].type = RELAP_POW2TH;
            }
            else if(word == "FERMI_XS2POW" || word == "fermi_xs2pow" || word == "Fermi_xs2pow"){
              sys->code[codeReaded].type = FERMI_XS2POW;
            }
            else if(word == "USER_CODE" || word == "user_code" || word == "User_code"){
              sys->code[codeReaded].type = USER_CODE;
            }
            else{
              error = NEWTON_ERROR;
              checkError(error,("Error reading TYPE card for code: "+codeReaded));
            }
            // TEST
            //~ cout<<"Type: "<<sys->code[codeReaded].id<<endl;
            //~ exit(1);
          }
          if(word=="CONNECTION"){
            configFile >> word;
            if(word == "MPI" || word == "mpi" || word == "Mpi"){
              sys->code[codeReaded].connection = NEWTON_MPI_COMMUNICATION;
            }
            else if(word == "SPAWN" || word == "spawn" || word == "Spawn"){
              sys->code[codeReaded].connection = NEWTON_SPAWN;
            }
            else if(word == "PPLEP" || word == "pplep" || word == "Pplep"){
              sys->code[codeReaded].connection = NEWTON_PPLEP;
            }
            else{
              sys->code[codeReaded].connection = NEWTON_UNKNOWN_CONNECTION;
              error = NEWTON_ERROR;
              checkError(error,("Error reading CONNECTION card for code: "+codeReaded));
            }
          }
          if(word=="N_PROCS"){
            configFile >> word;
            stringstream(word) >> sys->code[codeReaded].nProcs;
          }          
          if(word=="FROM_CODE_ID"){
            int iCode;
            configFile >> word;
            stringstream(word) >> iCode;
            // Dummy word VAR
            configFile >> word;
            configFile >> word;
            stringstream(word) >> sys->code[codeReaded].nCalculationsFromCode[iCode];
          }
          if(word=="TO_CODE_ID"){
            int iCode;
            configFile >> word;
            stringstream(word) >> iCode;
            // Dummy word VAR
            configFile >> word;
            configFile >> word;
            stringstream(word) >> sys->code[codeReaded].nCalculations2Code[iCode];
          }
          if(word=="GUESS_MAP"){
            configFile >> sys->code[codeReaded].map[NEWTON_PRE_SEND];
          }
          if(word=="CALC_MAP"){
            configFile >> sys->code[codeReaded].map[NEWTON_POST_RECV];
          }
          // Strings
          if(word=="INPUT_NAME"){
            configFile >> sys->code[codeReaded].inputModelName;
          }
          if(word=="INPUT_EXT"){
            configFile >> sys->code[codeReaded].inputExt;
          }
          if(word=="RESTART_NAME"){
            configFile >> sys->code[codeReaded].restartName;
          }
          if(word=="RESTART_EXT"){
            configFile >> sys->code[codeReaded].restartExt;
          }
          if(word=="RESTART_PATH"){
            configFile >> sys->code[codeReaded].restartPath;
          }
          if(word=="OUTPUT_NAME"){
            configFile >> sys->code[codeReaded].outputName;
          }
          if(word=="OUTPUT_EXT"){
            configFile >> sys->code[codeReaded].outputExt;
          }
          if(word=="OUTPUT_PATH"){
            configFile >> sys->code[codeReaded].outputPath;
          }
          if(word=="BIN_COMMAND"){
            configFile >> sys->code[codeReaded].binCommand;
          }
          if(word=="ARGS"){
            for(int iArg=0; iArg<sys->code[codeReaded].nArgs; iArg++){
              configFile >> sys->code[codeReaded].arg[iArg];
            }
          }
          
          configFile >> word;
        }
        codeReaded++;
            }
      
      // Next word
            configFile >> word;
        }
    }
    else{
        error = NEWTON_ERROR;
        checkError(error,"Error opening \"newton.config\"");
    }
  configFile.close();
}

/* Parser::isAComment

Analyze if the string is a comment.

input: string
output: true if it is a comment, negative other case

*/

bool Parser::isAComment(string word)
{
  if( word=="newton" ||
      word[0]=='#' ||
      word[0]=='@' ||
      word[0]=='%' ||
      word[0]=='_' ||
      word[0]=='-'){
    return true;
  }
  return false;
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
    for(int codeReaded=0; codeReaded<sys->nCodes; codeReaded++){
      if(sys->code[iCode].nCalculations2Code[codeReaded]
      != sys->code[codeReaded].nCalculationsFromCode[iCode]){
        rootPrints("Incompatible guesses and calculations.Check: "+int2str(iCode)+" -> "+int2str(codeReaded));
      }
    }
  }
   
  
    checkError(error,"Error checking consistency in config data");
}
