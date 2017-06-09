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
  // Empty word
  word = "";
  
  // Delimeter
  delim = "&";
  
  // Initial state for any guess
  unkSaved = false;
  
  // Initial error value
  error = NEWTON_SUCCESS;
}


/* Parser::wordIsCard
Ask if the word corresponds to one of the reserved cards for the parent.

input: string word, string parent
output: true in positive case, either false

*/
bool Parser::wordIsCard(string word, string parent)
{
  
  if(parent=="input"){
    if(word=="method"){
      return true;
    }
    if(word=="phases"){
      return true;
    }
    if(word=="abs_tol"){
      return true;
    }
    if(word=="max_iter"){
      return true;
    }
    if(word=="dx_jac_calc"){
      return true;
    }
    if(word=="steps_jac_calc"){
      return true;
    }
    if(word=="client"){
      return true;
    }
    if(word=="delta_step"){
      return true;
    }
    if(word=="n_steps"){
      return true;
    }
    if(word=="guesses"){
      return true;
    }
    if(word=="calcs"){
      return true;
    }
  }
  else if(parent=="client"){
    if(word=="guesses"){
      return true;
    }
    if(word=="calcs"){
      return true;
    }
    if(word=="input_name"){
      return true;
    }
    if(word=="input_ext"){
      return true;
    }
    if(word=="restart_name"){
      return true;
    }
    if(word=="restart_ext"){
      return true;
    }
    if(word=="restart_path"){
      return true;
    }
    if(word=="output_name"){
      return true;
    }
    if(word=="output_ext"){
      return true;
    }
    if(word=="output_path"){
      return true;
    }
    if(word=="bin_command"){
      return true;
    }
    if(word=="args"){
      return true;
    }
    if(word=="n_procs"){
      return true;
    }   
  }
  else if(parent=="guesses"){
    if(word=="map"){
      return true;
    }
    if(word=="guesses_mapped"){
      return true;
    }
  } 
  else if(parent=="calcs"){
    if(word=="map"){
      return true;
    }
    if(word=="calcs_pre_map"){
      return true;
    }
  } 
  
  return false;
}

/* Parser::takeNextWord
Get the next word and ask if it is a comment. In this case continue 
taking words after get a valid card.

input: -
output: word string

*/
string Parser::takeNextWord()
{
  // Next word
  configFile >> word;
  transform(word.begin(), word.end(), word.begin(), ::tolower);
  
  // Look for a comment
  while (isAComment(word)){          
    getline(configFile, word);
    // Next word
    configFile >> word;
    transform(word.begin(), word.end(), word.begin(), ::tolower);
  }
  
  return word;
}

/* Parser::checkImportantCards
Check if some important cards were set.

input: System pointer, Solver pointer
output: -

*/
void Parser::checkImportantCards(System* sys, Solver* sol)
{
  if(sys->nCodes==0){
    error = NEWTON_ERROR;
    checkError(error, "Set CLIENT cards - Parser::checkImportantCards");
  }
  if(sol->method==NO_METHOD){
    error = NEWTON_ERROR;
    checkError(error, "Set METHOD card - Parser::checkImportantCards");
  }
  if(sol->method==EXPLICIT_SERIAL){
    if(sys->nPhasesPerIter==0){
      error = NEWTON_ERROR;
      checkError(error, "Set PHASES card. Using EXPLICIT_SERIAL method - Parser::checkImportantCards");
    }     
  }
  
}

/* Parser::checkImportantProperties
Check if some important properties were set.

input: System pointer
output: -

*/
void Parser::checkImportantProperties(System* sys)
{
  if(sys->nUnk < sys->nRes){
    error = NEWTON_ERROR;
    checkError(error, "Bad formulation: too many residuals - Parser::checkImportantProperties");
  }  
}



/* Parser::parseInput
Parse the input file.

input: System pointer
output: -

*/
void Parser::parseInput(System* sys, Evolution* evol, Solver* sol)
{
  rootPrints("Parsing configuration file...");
  
  // First input file opening
  
  configFile.open("newton.configtest");
  if (configFile.is_open()){
    while(!configFile.eof()){
      
      // Count amount of codes in order to allocate
      
      if(word=="client"){
        sys->nCodes++;
        word = takeNextWord();
      }
      // Parsing numerical methods
      
      else if(word=="n_steps"){
        word = takeNextWord();
        stringstream(word) >> evol->nSteps;
        word = takeNextWord();
      }
      
      else if(word=="delta_step"){
        word = takeNextWord();
        stringstream(word) >> evol->deltaStep;
        word = takeNextWord();
      }
      
      else if(word=="abs_tol"){
        word = takeNextWord();
        stringstream(word) >> sol->nltol;
        word = takeNextWord();
      }
      
      else if(word=="max_iter"){
        word = takeNextWord();
        stringstream(word) >> sol->maxIter;
        word = takeNextWord();
      }
      
      else if(word=="method"){
        word = takeNextWord();
        stringstream(word) >> sol->method;
        word = takeNextWord();
      }
      
      else if(word=="dx_jac_calc"){
        word = takeNextWord();
        stringstream(word) >> sol->dxJacCalc;
        word = takeNextWord();
      }
      
      else if(word=="steps_jac_calc"){
        word = takeNextWord();
        stringstream(word) >> sol->sJacCalc;
        word = takeNextWord();
      }
      else if(word=="phases"){
        word="";
        while(!wordIsCard(word, "input") && !configFile.eof()){
          word = takeNextWord();
          if(word==delim){
            sys->nPhasesPerIter++;
          }          
        }
      }
      
      // Count amount of residuals
      
      else if(word=="calcs"){
        word = takeNextWord();
        while(!wordIsCard(word, "input") && !wordIsCard(word, "calcs") && !configFile.eof()){
          sys->nRes++;
          word = takeNextWord();
        }
      }
      
      else{
        word = takeNextWord();
      }
      
    }
  }
  else{
      error = NEWTON_ERROR;
      checkError(error,"Error opening \"newton.config\"");
  }
  configFile.close();
  
  // TEST
  //cout<<"nRes: "<<sys->nRes<<endl;
  
  // Check that all important cards are set
  
  checkImportantCards(sys, sol);
  
  // Allocate elements
  
  sys->allocate1(); // ********************************************change things
  sys->allocate2(); // ****************************************************** change names

  // Second input file opening
  
  configFile.open("newton.configtest");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Count amount of codes that run in each phase in EXPLICIT_SERIAL
      
      if(word=="phases"){
        for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          word = takeNextWord();
          while(word!=delim){
            sys->nCodesInPhase[iPhase]++;
            word = takeNextWord();
          }
        }
        word = takeNextWord();
      }
      
      
      else if(word=="client"){
        word = takeNextWord();
        if(!wordIsCard(word, "input") && !wordIsCard(word, "client") && !wordIsCard(word, "guesses") &&  !wordIsCard(word, "calcs")){
          sys->code2[clientReaded].name = word;
          word = takeNextWord();
        }
        else{
          error = NEWTON_ERROR;
          checkError(error, "Select a correct name for client. Word "+word+" is avoided - Parser::parseInput");
        }
        
        
        while(wordIsCard(word, "client") && !configFile.eof()){
          
          if(word=="calcs"){
            word = takeNextWord();
            // Reading string names of guesses variables
            do{
              if(!wordIsCard(word, "input") && !wordIsCard(word, "calcs") && !wordIsCard(word, "guesses") && !configFile.eof()){
                // Add one guess in alphas (default) and betas of the client
                sys->code2[clientReaded].nAlpha++;
                sys->code2[clientReaded].nBeta++;
                
                unkSaved = false;
                for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
                  if(sys->xName[iUnk]==word){
                    unkSaved = true;
                    break;
                  }
                }
                 // Save unknown names, and check if amount of unknowns == amount of residuals
                if(!unkSaved){
                  sys->nUnk++;
                  if(sys->nUnk > sys->nRes){
                    error = NEWTON_ERROR;
                    
                    checkError(error, "Bad formulation: too many unknowns - Parser::parseInput");
                  }
                  else{
                    sys->xName[sys->nUnk-1] = word;
                  }
                }
                word = takeNextWord();
              }
              else{
                error = NEWTON_ERROR;                  
                checkError(error, "Set name of calculated variables after CALCS property in CLIENT - Parser::parseInput");
              }
            }while(!wordIsCard(word, "input") && !wordIsCard(word, "calcs") && !wordIsCard(word, "guesses") && !configFile.eof());
            // Reading other properties of calcs
            while(!wordIsCard(word, "input") && !configFile.eof()){
              if(word=="map"){
                word = takeNextWord();
                sys->code2[clientReaded].alphaMap = word;
                word = takeNextWord();
              }
              else if(word=="calcs_pre_map"){
                word = takeNextWord();
                stringstream(word)>>sys->code2[clientReaded].nAlpha;
                word = takeNextWord();
              }
              else{
                error = NEWTON_ERROR;                  
                checkError(error, "Unknown property in CALCS - Parser::parseInput");
              }
            }
          }
          
          else if(word=="guesses"){
            word = takeNextWord();
            // Reading string names of guesses variables
            do{
              if(!wordIsCard(word, "input") && !wordIsCard(word, "calcs") && !wordIsCard(word, "guesses") && !configFile.eof()){
                // Add one guess in gammas and deltas (default) of the client
                sys->code2[clientReaded].nGamma++;
                sys->code2[clientReaded].nDelta++;
                
                unkSaved = false;
                for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
                  if(sys->xName[iUnk]==word){
                    unkSaved = true;
                    break;
                  }
                }
                 // Save unknown names, and check if amount of unknowns == amount of residuals
                if(!unkSaved){
                  sys->nUnk++;
                  if(sys->nUnk > sys->nRes){
                    error = NEWTON_ERROR;
                    
                    checkError(error, "Bad formulation: too many unknowns - Parser::parseInput");
                  }
                  else{
                    sys->xName[sys->nUnk-1] = word;
                  }
                }
                word = takeNextWord();
              }
              else{
                error = NEWTON_ERROR;                  
                checkError(error, "Set name of guesses variables after GUESSES property in CLIENT - Parser::parseInput");
              }
            }while(!wordIsCard(word, "input") && !wordIsCard(word, "calcs") && !wordIsCard(word, "guesses") && !configFile.eof());
            // Reading other properties of guesses
            while(!wordIsCard(word, "input") && !configFile.eof()){
              if(word=="map"){
                word = takeNextWord();
                sys->code2[clientReaded].gammaMap = word;
                word = takeNextWord();
              }
              else if(word=="guesses_mapped"){
                word = takeNextWord();
                stringstream(word)>>sys->code2[clientReaded].nDelta;
                word = takeNextWord();
              }
              else{
                error = NEWTON_ERROR;                  
                checkError(error, "Unknown property in CALCS - Parser::parseInput");
              }
            }
          }
          
          else if(word=="input_name"){
            word = takeNextWord();
            sys->code2[clientReaded].inputModelName = word;
            word = takeNextWord();
          }
          
          else if(word=="input_ext"){
            word = takeNextWord();
            sys->code2[clientReaded].inputExt = word;
            word = takeNextWord();
          }
          
          else if(word=="restart_name"){
            word = takeNextWord();
            sys->code2[clientReaded].restartName = word;
            word = takeNextWord();
          }
          
          else if(word=="restart_ext"){
            word = takeNextWord();
            sys->code2[clientReaded].restartExt = word;
            word = takeNextWord();
          }
          
          else if(word=="restart_path"){
            word = takeNextWord();
            sys->code2[clientReaded].restartPath = word;
            word = takeNextWord();
          }
          
          else if(word=="output_name"){
            word = takeNextWord();
            sys->code2[clientReaded].outputName = word;
            word = takeNextWord();
          }
          
          else if(word=="output_ext"){
            word = takeNextWord();
            sys->code2[clientReaded].outputExt = word;
            word = takeNextWord();
          }
          
          else if(word=="output_path"){
            word = takeNextWord();
            sys->code2[clientReaded].outputPath = word;
            word = takeNextWord();
          }
          
          else if(word=="bin_command"){
            word = takeNextWord();
            sys->code2[clientReaded].binCommand = word;
            word = takeNextWord();
          }
          
          else if(word=="connection"){
            word = takeNextWord();
            if(word=="mpi"){
              sys->code2[clientReaded].connection = NEWTON_MPI_COMMUNICATION;
            }
            else if(word=="spawn"){
              sys->code2[clientReaded].connection = NEWTON_SPAWN;
            }
            else if(word=="pplep"){
              sys->code2[clientReaded].connection = NEWTON_PPLEP;
            }
            else{
              error = NEWTON_ERROR;
              checkError(error, "Unknown connection type: "+word+" - Parser::parseInput");
            }
            
            
            sys->code2[clientReaded].binCommand = word;
            word = takeNextWord();
          }
          
          else if(word=="args"){
            word = takeNextWord();
            do{
              if(!wordIsCard(word, "input") && !wordIsCard(word, "calcs") && !wordIsCard(word, "guesses") && !configFile.eof()){
                sys->code2[clientReaded].nArgs++;
                word = takeNextWord();
              }
              else{
                error = NEWTON_ERROR;                  
                checkError(error, "Set arguments after ARGS property in CLIENT - Parser::parseInput");
              }
            }while(!wordIsCard(word, "input") && !wordIsCard(word, "calcs") && !wordIsCard(word, "guesses") && !configFile.eof());
          }
          
          else if(word=="n_procs"){
            word = takeNextWord();
            stringstream(word) >> sys->code2[clientReaded].nProcs;
            word = takeNextWord();
          }         
          
          else{
            error = NEWTON_ERROR;                  
            checkError(error, "Unknown property in CLIENT - Parser::parseInput");
          }
        } // Finish while(word inside client)
        clientReaded ++;
      }
      
      // Properties in bad order
      else if(wordIsCard(word, "calcs") || wordIsCard(word, "guesses")){
        error = NEWTON_ERROR;
        checkError(error, "Set CALCS and GUESSES properties after list variables. Word: "+word+" in bad place - Parser::parseInput");
      }
    
      else if(word=="dummy"){
        word = takeNextWord();
      }
      
      else{
        word = takeNextWord();
      }
    
    } // Finish while(!EOF)
    
  } 
  else{
      error = NEWTON_ERROR;
      checkError(error,"Error opening \"newton.config\"");
  }
  configFile.close();
  
  // Check that all important properties are set
  
  checkImportantProperties(sys);
  
  // Allocate elements
  
  sys->allocate3(); // ********************************************change things

  // TEST
  cout<<"Number of clients: "<<sys->nCodes<<endl;
  cout<<"Number of unknowns: "<<sys->nUnk<<" - Number of residuals: "<<sys->nRes<<endl;
  cout<<"Unknowns order: "<<endl;
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    cout<<sys->xName[iUnk]<<endl;
  }
  cout<<"Clients: "<<endl;
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    cout<<"Code: "<<sys->code2[iCode].name<<endl;
    cout<<" - nAlpha: "<<sys->code2[iCode].nAlpha<<endl;
    cout<<" - nBeta: "<<sys->code2[iCode].nBeta<<endl;
    cout<<" - nGamma: "<<sys->code2[iCode].nGamma<<endl;
    cout<<" - nDelta: "<<sys->code2[iCode].nDelta<<endl;
    cout<<" - alphaMap: "<<sys->code2[iCode].alphaMap<<endl;
    cout<<" - gammaMap: "<<sys->code2[iCode].gammaMap<<endl;
    cout<<" - n Procs: "<<sys->code2[iCode].nProcs<<endl;
    cout<<" - n Args: "<<sys->code2[iCode].nArgs<<endl;
    cout<<" - Command to run bin: "<<sys->code2[iCode].binCommand<<endl;
    cout<<" - Input model name: "<<sys->code2[iCode].inputModelName<<endl;
    cout<<" - Input extension: "<<sys->code2[iCode].inputExt<<endl;
    cout<<" - Restart name: "<<sys->code2[iCode].restartName<<endl;
    cout<<" - Restart extension: "<<sys->code2[iCode].restartExt<<endl;
    cout<<" - Output model name: "<<sys->code2[iCode].outputName<<endl;
    cout<<" - Output extension: "<<sys->code2[iCode].outputExt<<endl;
  }

















  exit(1);


  // Number of client codes readed
  clientReaded = 0;
  
  
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
            stringstream(word) >> sys->code[clientReaded].nArgs;
          }
          configFile >> word;
        }        
        clientReaded++;
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
  clientReaded = 0;
  
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
                stringstream(word) >> sol->sJacCalc;
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
            stringstream(word) >> sys->code[clientReaded].id;
          }          
          if(word=="TYPE"){
            configFile >> word;
            if(word == "TEST" || word == "test" || word == "Test"){
              sys->code[clientReaded].type = TEST;
            }
            else if(word == "RELAP_POW2TH" || word == "relap_pow2th" || word == "Relap_pow2th"){
              sys->code[clientReaded].type = RELAP_POW2TH;
            }
            else if(word == "FERMI_XS2POW" || word == "fermi_xs2pow" || word == "Fermi_xs2pow"){
              sys->code[clientReaded].type = FERMI_XS2POW;
            }
            else if(word == "USER_CODE" || word == "user_code" || word == "User_code"){
              sys->code[clientReaded].type = USER_CODE;
            }
            else{
              error = NEWTON_ERROR;
              checkError(error,("Error reading TYPE card for code: "+clientReaded));
            }
            // TEST
            //~ cout<<"Type: "<<sys->code[clientReaded].id<<endl;
            //~ exit(1);
          }
          if(word=="CONNECTION"){
            configFile >> word;
            if(word == "MPI" || word == "mpi" || word == "Mpi"){
              sys->code[clientReaded].connection = NEWTON_MPI_COMMUNICATION;
            }
            else if(word == "SPAWN" || word == "spawn" || word == "Spawn"){
              sys->code[clientReaded].connection = NEWTON_SPAWN;
            }
            else if(word == "PPLEP" || word == "pplep" || word == "Pplep"){
              sys->code[clientReaded].connection = NEWTON_PPLEP;
            }
            else{
              sys->code[clientReaded].connection = NEWTON_UNKNOWN_CONNECTION;
              error = NEWTON_ERROR;
              checkError(error,("Error reading CONNECTION card for code: "+clientReaded));
            }
          }
          if(word=="N_PROCS"){
            configFile >> word;
            stringstream(word) >> sys->code[clientReaded].nProcs;
          }          
          if(word=="FROM_CODE_ID"){
            int iCode;
            configFile >> word;
            stringstream(word) >> iCode;
            // Dummy word VAR
            configFile >> word;
            configFile >> word;
            stringstream(word) >> sys->code[clientReaded].nCalculationsFromCode[iCode];
          }
          if(word=="TO_CODE_ID"){
            int iCode;
            configFile >> word;
            stringstream(word) >> iCode;
            // Dummy word VAR
            configFile >> word;
            configFile >> word;
            stringstream(word) >> sys->code[clientReaded].nCalculations2Code[iCode];
          }
          if(word=="GUESS_MAP"){
            configFile >> sys->code[clientReaded].map[NEWTON_PRE_SEND];
          }
          if(word=="CALC_MAP"){
            configFile >> sys->code[clientReaded].map[NEWTON_POST_RECV];
          }
          // Strings
          if(word=="INPUT_NAME"){
            configFile >> sys->code[clientReaded].inputModelName;
          }
          if(word=="INPUT_EXT"){
            configFile >> sys->code[clientReaded].inputExt;
          }
          if(word=="RESTART_NAME"){
            configFile >> sys->code[clientReaded].restartName;
          }
          if(word=="RESTART_EXT"){
            configFile >> sys->code[clientReaded].restartExt;
          }
          if(word=="RESTART_PATH"){
            configFile >> sys->code[clientReaded].restartPath;
          }
          if(word=="OUTPUT_NAME"){
            configFile >> sys->code[clientReaded].outputName;
          }
          if(word=="OUTPUT_EXT"){
            configFile >> sys->code[clientReaded].outputExt;
          }
          if(word=="OUTPUT_PATH"){
            configFile >> sys->code[clientReaded].outputPath;
          }
          if(word=="BIN_COMMAND"){
            configFile >> sys->code[clientReaded].binCommand;
          }
          if(word=="ARGS"){
            for(int iArg=0; iArg<sys->code[clientReaded].nArgs; iArg++){
              configFile >> sys->code[clientReaded].arg[iArg];
            }
          }
          
          configFile >> word;
        }
        clientReaded++;
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
    for(int clientReaded=0; clientReaded<sys->nCodes; clientReaded++){
      if(sys->code[iCode].nCalculations2Code[clientReaded]
         != sys->code[clientReaded].nCalculationsFromCode[iCode]){
        rootPrints("Incompatible guesses and calculations.Check: "+int2str(iCode)+" -> "+int2str(clientReaded));
      }
    }
  }
   
  
    checkError(error,"Error checking consistency in config data");
}
