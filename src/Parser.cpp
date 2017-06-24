/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | PARSER
maste code            |
                      |

-------------------------------------------------------------------------------

Parser reads the input newton file and load all necessary data to solve the 
problem.

Date: 8 June 2017

-------------------------------------------------------------------------------

Copyright (C) 2017 Federico A. Caccia

This file is part of Newton.

Newton is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Newton is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Newton.  If not, see <http://www.gnu.org/licenses/>.

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
  
  // Initial amount of residuals loaded
  globalRes = 0;
  
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
  
  if(parent=="INPUT"){
    if(word=="METHOD"){
      return true;
    }
    if(word=="METHOD"){
      return true;
    }
    if(word=="PHASES"){
      return true;
    }
    if(word=="ABS_TOL"){
      return true;
    }
    if(word=="MAX_ITER"){
      return true;
    }
    if(word=="DX_JAC_CALC"){
      return true;
    }
    if(word=="X_EXT_ORDER"){
      return true;
    }
    if(word=="JAC_EXT_ORDER"){
      return true;
    }
    if(word=="STEPS_JAC_CALC"){
      return true;
    }
    if(word=="ITER_JAC_CALC"){
      return true;
    }
    if(word=="CLIENT"){
      return true;
    }
    if(word=="DELTA_STEP"){
      return true;
    }
    if(word=="N_STEPS"){
      return true;
    }
    if(word=="MAPPER"){
      return true;
    }
    if(word=="X_INI"){
      return true;
    }
    if(word=="DEBUG_TIME"){
      return true;
    }
    if(word=="DEBUG_PARSER"){
      return true;
    }
    if(word=="DEBUG_SYSTEM"){
      return true;
    }
    if(word=="DEBUG_CLIENT"){
      return true;
    }
    if(word=="DEBUG_EVOLUTION"){
      return true;
    }
    if(word=="DEBUG_SOLVER"){
      return true;
    }
    if(word=="DEBUG_MAPPER"){
      return true;
    }
    if(word=="DEBUG_COMM"){
      return true;
    }
  }
  else if(parent=="CLIENT"){
    if(word=="GUESSES"){
      return true;
    }
    if(word=="CALCS"){
      return true;
    }
    if(word=="INPUT_NAME"){      
      return true;
    }
    if(word=="INPUT_EXT"){
      return true;
    }
    if(word=="RESTART_NAME"){
      return true;
    }
    if(word=="RESTART_EXT"){
      return true;
    }
    if(word=="RESTART_PATH"){
      return true;
    }
    if(word=="OUTPUT_NAME"){
      return true;
    }
    if(word=="OUTPUT_EXT"){
      return true;
    }
    if(word=="OUTPUT_PATH"){
      return true;
    }
    if(word=="BIN_COMMAND"){
      return true;
    }
    if(word=="ARGS"){
      return true;
    }
    if(word=="N_PROCS"){
      return true;
    } 
    if(word=="CONNECTION"){
      return true;
    }
    if(word=="SPAWN_TYPE"){
      return true;
    }
    if(word=="X_INI"){
      return true;
    }
    if(word=="IO_TYPE"){
      return true;
    }
    if(word=="ID"){
      return true;
    }
    if(word=="N_PHYSICAL_ENTITIES"){
      return true;
    }
    if(word=="N_XS"){
      return true;
    }
    if(word=="N_GROUPS"){
      return true;
    }
    if(word=="PHYSICAL_ENTITIES"){
      return true;
    }
    if(word=="T_0"){
      return true;
    }
    if(word=="T_F"){
      return true;
    }
    if(word=="MAX_DT"){
      return true;
    }
    if(word=="HS_ZONES"){
      return true;
    }
    if(word=="PIPE_ZONES"){
      return true;
    }
    if(word=="N_RADIAL_POINTS"){
      return true;
    }
    if(word=="N_AXIAL_ZONES"){
      return true;
    }
  }
/*  else if(parent=="GUESSES"){
    if(word=="MAP"){
      return true;
    }
    if(word=="GUESSES_MAPPED"){
      return true;
    }
  } */
/*  else if(parent=="CALCS"){
    if(word=="MAP"){
      return true;
    }
    if(word=="CALCS_PRE_MAP"){
      return true;
    }
  } */
  else if(parent=="MAPPER"){
    if(word=="STAGE"){
      return true;
    }
    if(word=="MAP_CLIENT"){
      return true;
    }
    if(word=="CALCS_PRE_MAP"){
      return true;
    }
    if(word=="GUESSES_MAPPED"){
      return true;
    }
    if(word=="MAP_DATA"){
      return true;
    }
    if(word=="DATA_N_GROUPS"){
      return true;
    }
    if(word=="MATERIAL"){
      return true;
    }
    if(word=="ENERGY_PER_FISSION"){
      return true;
    }
  }
  else if(parent=="MATERIAL"){ 
     if(word=="N_BURNUP"){
      return true;
    }
    if(word=="PHYS_ENT"){
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
  //transform(word.begin(), word.end(), word.begin(), ::tolower);
  
  // Look for a comment
  while (isAComment(word) && !configFile.eof()){
    getline(configFile, word);
    // Next word
    configFile >> word;
    //transform(word.begin(), word.end(), word.begin(), ::tolower);
  }

  return word;
}

/* Parser::takeNextName
Get the next phrase until "&".

input: -
output: phrase string

*/
string Parser::takeNextName()
{
  getline(configFile, word, '&');

  // Erase blank spaces at first and end
  for(unsigned int i=0; i<word.length(); i++){
    if(word[0]==' '){
      word.erase(word.begin());
    }
    else{
      break;
    }
  }
  for(unsigned int i=0; i<word.length(); i++){
    if(*word.rbegin()==' '){
      word.erase(word.end()-1);
    }
    else{
      break;
    }
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
      checkError(error, "Using EXPLICIT_SERIAL method. PHASES card needed - Parser::checkImportantCards");
    }     
  }
  
}

/* Parser::checkConsistency
Check consistency between residuals and unknowns.

input: System pointer
output: -

*/
void Parser::checkConsistency(System* sys)
{
  if(sys->nUnk != sys->nRes){
    error = NEWTON_ERROR;
    checkError(error, "Bad formulation: residual equations: "+int2str(sys->nRes)+", unknowns: "+int2str(sys->nUnk)+"- Parser::checkConsistency");
  }  
}

/* Parser::wordIsForbidden

Analze if the word is forbidden or empty (EOF).

input: string word
output: bool true if it is forbidden, either false.

*/
bool Parser::wordIsForbidden(string word)
{
  if(wordIsCard(word, "INPUT") || 
     wordIsCard(word, "CLIENT") || 
/*     wordIsCard(word, "CALCS") || 
     wordIsCard(word, "GUESSES") ||*/
     wordIsCard(word, "MAPPER") ||
     configFile.eof()){
    return true;
  }

  return false;
}

/* Parser::loadCalcsAndTakeWord
Load all important data for calcs in a particular client and return the next word.

input: System pointer
output: next word readed

*/

string Parser::loadCalcsAndTakeWord(System* sys)
{
  word = takeNextWord();
  
  // Read string names of calcs variables
  
  do{
    if(!wordIsForbidden(word)){
        // Add one guess in alphas (default) and betas of the client
        sys->code[clientReaded].nAlpha++;
        sys->code[clientReaded].nBeta++;
        
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
            
            checkError(error, "Bad formulation: too many unknowns: \""+word+"\" - Parser::loadCalcsAndTakeWord");
          }
          else{
            sys->xName[sys->nUnk-1] = word;
          }
        }
        word = takeNextWord();
      }
    }while(!wordIsForbidden(word));
  
  // Reading other properties of calcs

/*  while(wordIsCard(word, "CALCS")){
    if(word=="MAP"){
      word = takeNextWord();
      if(!wordIsForbidden(word)){
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        sys->code[clientReaded].alphaMap = word;
      }
      else{
        error = NEWTON_ERROR;
        checkError(error, "\""+word+"\" word is forbidden to use as mapper - Parser::loadCalcsAndTakeWord");
      }
      word = takeNextWord();
    }
    else if(word=="CALCS_PRE_MAP"){
      word = takeNextWord();
      stringstream(word)>>sys->code[clientReaded].nAlpha;
      word = takeNextWord();
    }
  }*/

  return word;
}

/* Parser::loadGuessesAndTakeWord
Load all important data for guesses in a particular client and return the next word.

input: System pointer
output: next word readed

*/

string Parser::loadGuessesAndTakeWord(System* sys)
{

  word = takeNextWord();
  
  // Read string names of guesses variables
  
  do{
    if(!wordIsForbidden(word)){
        // Add one guess in gammas and deltas (default) of the client
        sys->code[clientReaded].nGamma++;
        sys->code[clientReaded].nDelta++;
        
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
            
            checkError(error, "Bad formulation: too many unknowns: \""+word+"\" - Parser::loadGuessesAndTakeWord");
          }
          else{
            sys->xName[sys->nUnk-1] = word;
          }
        }
        word = takeNextWord();        
      }
    }while(!wordIsForbidden(word));
  
  // Reading other properties of calcs
  
/*  while(wordIsCard(word, "GUESSES")){
    if(word=="MAP"){
      word = takeNextWord();
      if(!wordIsForbidden(word)){
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        sys->code[clientReaded].gammaMap = word;
      }
      else{
        error = NEWTON_ERROR;
        checkError(error, "\""+word+"\" word is forbidden to use as mapper - Parser::loadGuessesAndTakeWord");
      }
      word = takeNextWord();
    }
    else if(word=="GUESSES_MAPPED"){
      word = takeNextWord();
      stringstream(word)>>sys->code[clientReaded].nDelta;
      word = takeNextWord();
    }
  }*/

  return word;
}

/* Parser::loadClientAndTakeWord
Load all important data for a particular client and return the next word.

input: System pointer
output: next word readed

*/

string Parser::loadClientAndTakeWord(System* sys)
{

  word = takeNextWord();

  // Take name

  if(!wordIsForbidden(word)){
    sys->code[clientReaded].name = word;
    word = takeNextWord();
  }
  else{
    error = NEWTON_ERROR;
    checkError(error, "Select a correct name for client. Word "+word+" is avoided - Parser::loadClientAndTakeWord");
  }

  // Take properties

  while(wordIsCard(word, "CLIENT") && !configFile.eof()){

    if(word=="ID"){
      word = takeNextWord();
      stringstream(word) >> sys->code[clientReaded].id;
      word = takeNextWord();
    }
    
    else if(word=="IO_TYPE"){
      word = takeNextWord();
      transform(word.begin(), word.end(), word.begin(), ::tolower);
      if(word == "test"){
         sys->code[clientReaded].type = TEST;
      }
      else if(word == "user_code"){
         sys->code[clientReaded].type = USER_CODE;
      }
      else if(word == "relap_pow2th"){
         sys->code[clientReaded].type = RELAP_POW2TH;
      }
      else if(word == "fermi_xs2pow"){
         sys->code[clientReaded].type = FERMI_XS2POW;
      }
      else{
        error = NEWTON_ERROR;
        cout<<"Client code tye:\""<< word<<"\"not founded - Parser::loadClientAndTakeWord"<<endl;
      }
      word = takeNextWord();
    }
    
    else if(word=="SPAWN_TYPE"){
      word = takeNextWord();
      transform(word.begin(), word.end(), word.begin(), ::tolower);
      if(word=="mpi"){
        sys->code[clientReaded].spawnType = word;
      }
      word = takeNextWord();
    }
          
    else if(word=="CALCS"){
      word = loadCalcsAndTakeWord(sys);
    }
    
    else if(word=="GUESSES"){
      word = loadGuessesAndTakeWord(sys);
    }
    
    else if(word=="INPUT_NAME"){
      word = takeNextWord();
      sys->code[clientReaded].inputModelName = word;
      word = takeNextWord();
    }
    
    else if(word=="INPUT_EXT"){
      word = takeNextWord();
      sys->code[clientReaded].inputExt = word;
      word = takeNextWord();
    }
    
    else if(word=="RESTART_NAME"){
      word = takeNextWord();
      sys->code[clientReaded].restartName = word;
      word = takeNextWord();
    }
    
    else if(word=="RESTART_EXT"){
      word = takeNextWord();
      sys->code[clientReaded].restartExt = word;
      word = takeNextWord();
    }
    
    else if(word=="RESTART_PATH"){
      word = takeNextWord();
      sys->code[clientReaded].restartPath = word;
      word = takeNextWord();
    }
    
    else if(word=="OUTPUT_NAME"){
      word = takeNextWord();
      sys->code[clientReaded].outputName = word;
      word = takeNextWord();
    }
    
    else if(word=="OUTPUT_EXT"){
      word = takeNextWord();
      sys->code[clientReaded].outputExt = word;
      word = takeNextWord();
    }
    
    else if(word=="OUTPUT_PATH"){
      word = takeNextWord();
      sys->code[clientReaded].outputPath = word;
      word = takeNextWord();
    }
    
    else if(word=="BIN_COMMAND"){
      word = takeNextWord();
      while(!wordIsForbidden(word)){
        sys->code[clientReaded].binCommand += word+" ";
        word = takeNextWord();
      }
    }
    
    else if(word=="CONNECTION"){
      word = takeNextWord();
      transform(word.begin(), word.end(), word.begin(), ::toupper);

      if(word=="MPI"){
        sys->code[clientReaded].connection = NEWTON_MPI_COMMUNICATION;
      }
      else if(word=="IO"){
        sys->code[clientReaded].connection = NEWTON_SPAWN;
      }
      else if(word=="PPLEP"){
        sys->code[clientReaded].connection = NEWTON_PPLEP;
      }
      else{
        error = NEWTON_ERROR;
        checkError(error, "Unknown connection IO_TYPE: "+word+" - Parser::loadClientAndTakeWord");
      }
      word = takeNextWord();
    }
    
    else if(word=="ARGS"){
      word = takeNextWord();
      do{
        if(!wordIsForbidden(word)){
          sys->code[clientReaded].nArgs++;
          word = takeNextWord();
        }
        else{
          error = NEWTON_ERROR;                  
          checkError(error, "Set arguments after ARGS property in CLIENT - Parser::loadClientAndTakeWord");
        }
      }while(!wordIsForbidden(word));
    }
    
    else if(word=="N_PROCS"){
      word = takeNextWord();
      stringstream(word) >> sys->code[clientReaded].nProcs;
      word = takeNextWord();
    }
    
    else if(wordIsCard(word, "CLIENT")){
      word = takeNextWord();
      while(!wordIsForbidden(word)){
        word = takeNextWord();
      }
    }

    else{
      word = takeNextWord();
    }
    
  } // Finish while(word inside client)

  // Bad cases

  /*if(!wordIsForbidden(word)){
    error = NEWTON_ERROR;
    checkError(error, "Unknown input word: \""+word+"\" - Parser::loadClientAndTakeWord");
  }*/

  // Bad position of guesses and calcs properties

/*  if(wordIsCard(word, "GUESSES") ||
     wordIsCard(word, "CALCS")){
    error = NEWTON_ERROR;
    checkError(error, "Bad place for input card: \""+word+"\" - Parser::loadClientAndTakeWord");
  }*/
  
  checkClientProperties(sys, clientReaded);

  clientReaded ++;

  return word;
}

/* Parser::checkClientProperties
Looks if all important cards are set in client.

input: System pointer, client number
output: -

*/
void Parser::checkClientProperties(System* sys, int client)
{
  // Connection type

  if(sys->code[client].connection==NEWTON_UNKNOWN_CONNECTION){
    error = NEWTON_ERROR;
    checkError(error, "CONNECTION unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
  }

  // I/O connection type

  if(sys->code[client].connection==NEWTON_SPAWN){
    if(sys->code[client].inputModelName==""){
      error = NEWTON_ERROR;
      checkError(error, "INPUT_NAME unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
    }
    if(sys->code[client].inputExt==""){
      error = NEWTON_ERROR;
      checkError(error, "INPUT_EXT unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
    }
    if(sys->code[client].binCommand==""){
      error = NEWTON_ERROR;
      checkError(error, "BIN_COMMAND unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
    }
  }

  // Variables
  if(sys->code[client].nAlpha==0){
    error = NEWTON_ERROR;
    checkError(error, "CALCS_PRE_MAP unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
  }
  if(sys->code[client].nBeta==0){
    error = NEWTON_ERROR;
    checkError(error, "CALCS unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
  }
  if(sys->code[client].nGamma==0){
    error = NEWTON_ERROR;
    checkError(error, "GUESSES unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
  }
  if(sys->code[client].nDelta==0){
    error = NEWTON_ERROR;
    checkError(error, "GUESSES_MAPPED unknown in client: "+sys->code[client].name+ " - Parser::checkClientProperties");
  }

}


/* Parser::parseInput
Parse the input file.

input: System pointer
output: -

*/
void Parser::parseInput(System* sys, Evolution* evol, Solver* sol, Client* client, Mapper* mapper, Communicator* comm, Debugger* newtonDebugger)
{
  rootPrints("Parsing configuration file...");

  // First input file opening
  
  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){
      
      // Look at debugger options
      if(word=="DEBUG_TIME"){
        newtonDebugger->setOn();
        word = takeNextWord();
      }
      else if(word=="DEBUG_PARSER"){
        debug.setOn();
        word = takeNextWord();
      }
      else if(word=="DEBUG_CLIENT"){
        client->debug.setOn();
        word = takeNextWord();
      }
      else if(word=="DEBUG_SYSTEM"){
        sys->debug.setOn();
        word = takeNextWord();
      }
      else if(word=="DEBUG_EVOLUTION"){
        evol->debug.setOn();
        word = takeNextWord();
      }
      else if(word=="DEBUG_SOLVER"){
        word = takeNextWord();
        do{
          transform(word.begin(), word.end(), word.begin(), ::tolower);
          if(word=="global"){
            sol->debug.setOn(GLOBAL_LOG);  
          }
          else if(word=="iter"){
            sol->debug.setOn(ITER_LOG);  
          }
          else if(word=="res"){
            sol->debug.setOn(RES_LOG);  
          }
          else if(word=="x"){
            sol->debug.setOn(X_LOG);  
          }
          else if(word=="j"){
            sol->debug.setOn(J_LOG);  
          }
          else if(word=="unk"){
            sol->debug.setOn(UNK_LOG);  
          }
          else{
            error = NEWTON_ERROR;
            checkError(error, "ERROR. Bad debbuger: \""+word+"\" selected in DEBUG_SOLVER - Parser::parseInput");
          }
          word = takeNextWord();
        }while(!wordIsForbidden(word));        
      }
      else if(word=="DEBUG_MAPPER"){
        mapper->debug.setOn();
        word = takeNextWord();
      }
      else if(word=="DEBUG_COMM"){
        comm->debug.setOn();
        word = takeNextWord();
      }


      // Count amount of codes in order to allocate

      else if(word=="CLIENT"){
        sys->nCodes++;
        word = takeNextWord();
        // Client name
        word = takeNextWord();
      }

      // Parsing numerical methods
      
      else if(word=="N_STEPS"){
        word = takeNextWord();
        stringstream(word) >> evol->nSteps;
        if(evol->nSteps<1){
          error = NEWTON_ERROR;
          checkError(error, "Bad number of steps: \""+word+"\" in N_STEPS - Parser::parseInput");
        }
        word = takeNextWord();
      }
      
      else if(word=="DELTA_STEP"){
        word = takeNextWord();
        stringstream(word) >> evol->deltaStep;
        word = takeNextWord();
      }
      
      else if(word=="ABS_TOL"){
        word = takeNextWord();
        stringstream(word) >> sol->nltol;
        word = takeNextWord();
      }
      
      else if(word=="MAX_ITER"){
        word = takeNextWord();
        stringstream(word) >> sol->maxIter;
        word = takeNextWord();
      }
      
      else if(word=="X_EXT_ORDER"){
        word = takeNextWord();
        int aux;
        stringstream(word) >> aux;
        if(aux>1){
          error = NEWTON_ERROR;
          checkError(error, "ERROR. X extrapolation only admit order 1 - Parser::parseInput");
        }
        else if(aux>=0){
          sol->nXStPrev = aux + 1;
        }
        else{
          error = NEWTON_ERROR;
          checkError(error, "ERROR. X extrapolation only admit positive order - Parser::parseInput");
        }
        word = takeNextWord();
      }
      else if(word=="J_EXT_ORDER"){
        word = takeNextWord();
        int aux;
        stringstream(word) >> aux;
        if(aux>1){
          error = NEWTON_ERROR;
          checkError(error, "ERROR. J extrapolation only admit order 1 - Parser::parseInput");
        }
        else if(aux>=0){
          sol->nJStPrev = aux + 1;
        }
        else{
          error = NEWTON_ERROR;
          checkError(error, "ERROR. J extrapolation only admit positive order - Parser::parseInput");
        }
        word = takeNextWord();
      }
      else if(word=="METHOD"){
        word = takeNextWord();
        transform(word.begin(), word.end(), word.begin(), ::toupper);
        if(word=="EXPLICIT_SERIAL"){
          sol->method = EXPLICIT_SERIAL;
        }
        else if(word=="EXPLICIT_PARALLEL"){
          sol->method = EXPLICIT_PARALLEL;
        }
        else if(word=="NEWTON"){
          sol->method = NEWTON;
        }
        else if(word=="SECANT"){
          sol->method = SECANT;
        }
        else if(word=="BROYDEN"){
          sol->method = BROYDEN;
        }       
        else{
          error = NEWTON_ERROR;
          checkError(error, "Unknown method: \""+word+"\" - Parser::parseInput");
        }
        word = takeNextWord();
      }
      
      else if(word=="DX_JAC_CALC"){
        word = takeNextWord();
        stringstream(word) >> sol->dxJacCalc;
        word = takeNextWord();
      }
      
      else if(word=="STEPS_JAC_CALC"){
        word = takeNextWord();
        stringstream(word) >> sol->sJacCalc;
        word = takeNextWord();
      }
      
      else if(word=="ITER_JAC_CALC"){
        word = takeNextWord();
        stringstream(word) >> sol->iJacCalc;
        word = takeNextWord();
      }

      else if(word=="PHASES"){
        word = takeNextWord();
        while(!wordIsForbidden(word)){
          if(word==delim){
            sys->nPhasesPerIter++;         
          }
          word = takeNextWord();          
        }
        if(sys->nPhasesPerIter==0){
          error = NEWTON_ERROR;
          checkError(error, "Bad PHASES enumeration - Parser::parseInput");
        }
      }
      
      // Count amount of residuals (and total betas)
      
      else if(word=="CALCS"){
        word = takeNextWord();
        do{
          if(wordIsForbidden(word)){
            error = NEWTON_ERROR;
            checkError(error, "Bad CALCS enumeration - Parser::parseInput");
          }
          sys->nRes++;
          sys->nBeta++;
          word = takeNextWord();
        }while(!wordIsForbidden(word));
      }

      // Count amount of guesses (and total gammas)
      
      else if(word=="GUESSES"){
        word = takeNextWord();
        do{
          if(wordIsForbidden(word)){
            error = NEWTON_ERROR;
            checkError(error, "Bad GUESSES enumeration - Parser::parseInput");
          }
          sys->nGamma++;
          word = takeNextWord();
        }while(!wordIsForbidden(word));
      }

      // Unknown word

      /*else if(!wordIsForbidden(word) && word != ""){
        error = NEWTON_ERROR;
        checkError(error, "Unknown input word: \""+word+"\" - Parser::parseInput");
      }*/

      // Unknown or empty word
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
  sys->allocate1(sol->method);

  // Second input file opening
  
  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Count amount of codes that run in each phase in EXPLICIT_SERIAL
      
      if(word=="PHASES" && sol->method == EXPLICIT_SERIAL){
        for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          word = takeNextWord();
          while(word!=delim){
            sys->nCodesInPhase[iPhase]++;
            word = takeNextWord();
          }
        }
        word = takeNextWord();
      }

      else if(word=="X_ORDER"){
        word = takeNextWord(); 
      }
      
      else if(word=="RES_ORDER"){
        word = takeNextWord(); 
      }
      
      // Analyze all properties of the client and return the word out of that

      else if(word=="CLIENT"){        
        word = loadClientAndTakeWord(sys);        
      }
      
      // Properties in bad order
      else if(wordIsCard(word, "CLIENT") && !wordIsCard(word, "INPUT")){
        error = NEWTON_ERROR;
        checkError(error, "Word in bad place. Set "+word+" after CLIENT - Parser::parseInput");
      }
/*      else if(wordIsCard(word, "CALCS") || wordIsCard(word, "GUESSES")){
        error = NEWTON_ERROR;
        checkError(error, "Set CALCS and GUESSES properties after list variables. Word: "+word+" in bad place - Parser::parseInput");
      }*/
    
      else if(word=="DUMMY"){
        word = takeNextWord();
      }

      // Unknown word

      /*else if(!wordIsForbidden(word) && word != ""){
        error = NEWTON_ERROR;
        checkError(error, "Unknown input word: \""+word+"\" - Parser::parseInput");
      }*/

      // Unknown or empty word

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
  
  checkConsistency(sys);
  
  // Allocate elements
  
  sys->allocate3(); // ********************************************change things

  //TEST
  //~ cout<<"Number of clients: "<<sys->nCodes<<endl;
  //~ cout<<"Number of unknowns: "<<sys->nUnk<<" - Number of residuals: "<<sys->nRes<<endl;
  //~ cout<<"Unknowns order: "<<endl;
  //~ for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    //~ cout<<sys->xName[iUnk]<<endl;
  //~ }
  //~ cout<<"Nonlinear method: "<<sol->method<<endl;
  //~ cout<<"Phases: "<<sys->nPhasesPerIter<<endl;
  //~ for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
    //~ cout<<" "<<iPhase<<"- n codes in phase: "<<sys->nCodesInPhase[iPhase]<<endl;
  //~ }
  //~ cout<<"Clients: "<<endl;
  //~ for(int iCode=0; iCode<sys->nCodes; iCode++){
    //~ cout<<"Code: "<<sys->code[iCode].name<<endl;
    //~ cout<<" - nAlpha: "<<sys->code[iCode].nAlpha<<endl;
    //~ cout<<" - nBeta: "<<sys->code[iCode].nBeta<<endl;
    //~ cout<<" - nGamma: "<<sys->code[iCode].nGamma<<endl;
    //~ cout<<" - nDelta: "<<sys->code[iCode].nDelta<<endl;
    //~ cout<<" - alphaMap: "<<sys->code[iCode].alphaMap<<endl;
    //~ cout<<" - gammaMap: "<<sys->code[iCode].gammaMap<<endl;
    //~ cout<<" - n Procs: "<<sys->code[iCode].nProcs<<endl;
    //~ cout<<" - n Args: "<<sys->code[iCode].nArgs<<endl;
    //~ cout<<" - connection type: "<<sys->code[iCode].connection<<endl;
    //~ cout<<" - command to run bin: "<<sys->code[iCode].binCommand<<endl;
    //~ cout<<" - Input model name: "<<sys->code[iCode].inputModelName<<endl;
    //~ cout<<" - Input extension: "<<sys->code[iCode].inputExt<<endl;
    //~ cout<<" - Restart name: "<<sys->code[iCode].restartName<<endl;
    //~ cout<<" - Restart extension: "<<sys->code[iCode].restartExt<<endl;
    //~ cout<<" - Output model name: "<<sys->code[iCode].outputName<<endl;
    //~ cout<<" - Output extension: "<<sys->code[iCode].outputExt<<endl;
  //~ }

  // Reinitialization
  betaLoaded = 0;
  gammaLoaded = 0;
  clientReaded =0;
  
  // Third input file opening  

  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Read clients that run in each phase
      
      if(word=="PHASES" && sol->method == EXPLICIT_SERIAL){
        for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
            word = takeNextWord();
            sys->codeToConnectInPhase[iPhase][iCode] = word;
          }
          // Take "&"
          word = takeNextWord();
        }
        word = takeNextWord();
        
        checkConsistencyInPhases(sys);
      }      
      
      // Read variables

      else if(word=="CLIENT"){
        for(int loading=0; loading<2; loading++){
          while(word!="CALCS" && word!= "GUESSES"){
            word = takeNextWord();
          }
          if(word=="CALCS"){
            for(int iBeta=0; iBeta<sys->code[clientReaded].nBeta; iBeta++){
              word = takeNextWord();
              sys->betaName[betaLoaded] = word;
              betaLoaded++;
              // Save residual name
              sys->resName[globalRes] = word;
              globalRes++;
            }
          }
          else if(word=="GUESSES"){
            for(int iGamma=0; iGamma<sys->code[clientReaded].nGamma; iGamma++){
              word = takeNextWord();
              sys->gammaName[gammaLoaded] = word;
              gammaLoaded++;
            }
          }
        }        
        clientReaded++;
      }
    
      // Unknown or empty word

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

  // Reinitialization

  clientReaded = 0;

  // Fourth input file opening  

  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Read args
      if(word=="CLIENT"){
        if(sys->code[clientReaded].nArgs>0){          
          while(word!="ARGS"){
            word = takeNextWord();
          }
          if(word=="ARGS"){
            for(int iArg=0; iArg<sys->code[clientReaded].nArgs; iArg++){
              word = takeNextWord();
              sys->code[clientReaded].arg[iArg] = word;
            }
          }
        }
        word = takeNextWord();
        clientReaded++;
      }

      // Unknown or empty word

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

  // TEST

  //~ cout<<"Codes to connect in each phase in EXPLICIT_SERIAL"<<endl;
  //~ for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
    //~ for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
      //~ cout<<" - "<<sys->codeToConnectInPhase[iPhase][iCode]<<" ";
    //~ }
    //~ cout<<endl;
  //~ }
//~ 
  //~ cout<<"Global betas: "<<sys->nBeta<<endl;
  //~ for(int iBeta=0; iBeta<sys->nBeta; iBeta++){
    //~ cout<<" - "<<sys->betaName[iBeta]<<": "<<sys->beta[iBeta]<<endl;
  //~ }
  //~ cout<<"Global gammas: "<<sys->nGamma<<endl;
  //~ for(int iGamma=0; iGamma<sys->nGamma; iGamma++){
    //~ cout<<" - "<<sys->gammaName[iGamma]<<": "<<sys->gamma[iGamma]<<endl;
  //~ }
//~ 
  //~ cout<<" Arguments"<<endl;
  //~ for(int iCode=0; iCode<sys->nCodes; iCode++){
    //~ cout<<" ("<<sys->code[iCode].nArgs<<"): ";
    //~ for(int iArg=0; iArg<sys->code[iCode].nArgs; iArg++){
      //~ cout<<sys->code[iCode].arg[iArg]<<" ";
    //~ }
    //~ cout<<endl;
  //~ }

  
  
  // Load order of x and order of residuals
  // xName should be updated on second input file opening
  // (and also resName)
  
  // Load initial conditions
  
  // Allocate x
  sol->initialize(sys);
  
  // Fifth input file opening  

  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Load CI on solver values of x and J
      if(word=="X_INI"){
        word = takeNextWord();
        while(!wordIsForbidden(word)){
          bool ciLoaded = false;
          for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
            if(word==sys->xName[iUnk]){
              word = takeNextWord();
              stringstream(word) >> sol->x[iUnk];
              ciLoaded = true;
              break;
            }
          }
            
          if(ciLoaded == false){
            error = NEWTON_ERROR;
            checkError(error, "Unable to find: \""+word+"\" reading X_INI - Parser::parseInput");
          }
          word = takeNextWord();
        }
      }

      // Unknown or empty word

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
  
  //~ // TEST
  //~ cout<<"Initial conditions:"<<endl;
  //~ for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    //~ cout<<sys->xName[iUnk]<<": "<<sol->x[iUnk]<<endl;
  //~ }
  // Count amount of each code type
  
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    if(sys->code[iCode].type==FERMI_XS2POW){
      client->nFermi++;
    }
    else if(sys->code[iCode].type==RELAP_POW2TH){
      client->nRelap++;
    }
  }

  // Allocate things in clients
  client->allocate1();
  
  // Initialize counters
  clientReaded = 0;
  fermiReaded = 0;
  relapReaded = 0;
  
  // Set client names
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    if(sys->code[iCode].type==FERMI_XS2POW){
      client->fermi[fermiReaded].name = sys->code[iCode].name;
      fermiReaded++;
    }
    else if(sys->code[iCode].type==RELAP_POW2TH){
      client->relap[relapReaded].name = sys->code[iCode].name;
      relapReaded++;
    }
  }
  
  // Initialize counters  
  clientReaded = 0;
  fermiReaded = 0;
  relapReaded = 0;
  
  // CODE SPECIFIC READINGS

  // Sixth input file opening  

  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){
      //cout<<word<<endl;
      // Load FERMI amount of things
      if(word=="CLIENT"){
        
        // FERMI_XS2POW
        if(sys->code[clientReaded].type==FERMI_XS2POW){
          word = takeNextWord();
          while((!wordIsCard(word, "INPUT") || (wordIsCard(word, "INPUT") && wordIsCard(word, "CLIENT"))) && !configFile.eof()){
            if(word=="N_PHYSICAL_ENTITIES"){
              word = takeNextWord();
              stringstream(word) >> client->fermi[fermiReaded].nPhysicalEntities;
            }
            else if(word=="N_GROUPS"){
              word = takeNextWord();
              stringstream(word) >> client->fermi[fermiReaded].nGroups;
            }
            else if(word=="N_XS"){
              word = takeNextWord();
              stringstream(word) >> client->fermi[fermiReaded].nXS;
            }           
            word = takeNextWord();
          }

          fermiReaded++;
        }
        
        // RELAP_POW2TH
        else if(sys->code[clientReaded].type==RELAP_POW2TH){
          word = takeNextWord();

          while((!wordIsCard(word, "INPUT") || (wordIsCard(word, "INPUT") && wordIsCard(word, "CLIENT"))) && !configFile.eof()){
            
            if(word=="T_0"){
              word = takeNextWord();
              stringstream(word) >> client->relap[relapReaded].t0;
            }
            else if(word=="T_F"){
              word = takeNextWord();
              stringstream(word) >> client->relap[relapReaded].tf;
            }
            else if(word=="MAX_DT"){
              word = takeNextWord();
              stringstream(word) >> client->relap[relapReaded].maxDt;
            }
            else if(word=="N_AXIAL_ZONES"){
              word = takeNextWord();
              stringstream(word) >> client->relap[relapReaded].nAxialZones;
            }
            else if(word=="N_RADIAL_POINTS"){
              word = takeNextWord();
              stringstream(word) >> client->relap[relapReaded].nRadialPoints;
            }
            word = takeNextWord();
            //cout<<word<<endl;
          }
          relapReaded++;
        }
        else{
          word = takeNextWord();
        }
        clientReaded++;
      }
      // Unknown or empty word

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
  
  // Allocate things in clients
  client->allocate2();

  // Initialize counters
  clientReaded = 0;
  fermiReaded = 0;
  relapReaded = 0;
  
  // CODE SPECIFIC READINGS

  // Sixth input file opening  

  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Load FERMI amount of things
      if(word=="CLIENT"){
        
        if(sys->code[clientReaded].type==FERMI_XS2POW){
          word = takeNextWord();
          while((!wordIsCard(word, "INPUT") || (wordIsCard(word, "INPUT") && wordIsCard(word, "CLIENT"))) && !configFile.eof()){
            
            if(word=="PHYSICAL_ENTITIES"){
              for(int ipe=0; ipe<client->fermi[fermiReaded].nPhysicalEntities; ipe++){
                word = takeNextWord();
                client->fermi[fermiReaded].pe[ipe].name = word;
               }
            }
            word = takeNextWord();
            
          }
          fermiReaded++;
        }        
        else if(sys->code[clientReaded].type==RELAP_POW2TH){
          word = takeNextWord();
         while((!wordIsCard(word, "INPUT") || (wordIsCard(word, "INPUT") && wordIsCard(word, "CLIENT"))) && !configFile.eof()){

            if(word=="HS_ZONES"){
              for(int ihs=0; ihs<client->relap[relapReaded].nAxialZones; ihs++){
                word = takeNextWord();
                client->relap[relapReaded].hs[ihs] = word;
               }
            }
            if(word=="PIPE_ZONES"){
              for(int ip=0; ip<client->relap[relapReaded].nAxialZones; ip++){
                word = takeNextWord();
                client->relap[relapReaded].pipe[ip] = word;
               }
            }
            word = takeNextWord();
            
          }
          relapReaded++;
        }
        else{
          word = takeNextWord();     
        }
        clientReaded++;
      }
      // Unknown or empty word

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
  

  
  //~ // TEST
  // Initialize counters  
  //~ fermiReaded = 0;
  //~ relapReaded = 0;
  //~ for(int iCode=0; iCode<sys->nCodes; iCode++){
    //~ if(sys->code[iCode].type==FERMI_XS2POW){
      //~ cout<<client->fermi[fermiReaded].name<<endl;
      //~ cout<<"n physical entities: "<<client->fermi[fermiReaded].nPhysicalEntities<<endl;
      //~ cout<<"n energy groups: "<<client->fermi[fermiReaded].nGroups<<endl;
      //~ cout<<"n XS: "<<client->fermi[fermiReaded].nXS<<endl;
      //~ for(int ipe=0; ipe<client->fermi[fermiReaded].nPhysicalEntities; ipe++){
        //~ cout<<" - pe: "<<client->fermi[fermiReaded].pe[ipe].name<<endl;
      //~ }     
      //~ fermiReaded++;
    //~ }
    //~ else if(sys->code[iCode].type==RELAP_POW2TH){
      //~ cout<<client->relap[relapReaded].name<<endl;
      //~ cout<<"t0: "<<client->relap[relapReaded].t0<<endl;
      //~ cout<<"tf: "<<client->relap[relapReaded].tf<<endl;
      //~ cout<<"max dt: "<<client->relap[relapReaded].maxDt<<endl;
      //~ cout<<"n radial points: "<<client->relap[relapReaded].nRadialPoints<<endl;
      //~ cout<<"n axial zones: "<<client->relap[relapReaded].nAxialZones<<endl;
      //~ for(int iZone=0; iZone<client->relap[relapReaded].nAxialZones; iZone++){
        //~ cout<<" - hs: "<<client->relap[relapReaded].hs[iZone]<<" pipe: "<<client->relap[relapReaded].pipe[iZone]<<endl;
      //~ }
      //~ relapReaded++;
    //~ }
  //~ }
  //~ exit(1);
  
  // MAPPER READINGS

  // Seventh input file opening

  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Count things in mapper
      if(word=="MAPPER"){

        string mapName;
        string clientName = "";
        int stage=-1;
        int nPreMap = -1;
        int nPostMap = -1;
        mapName = takeNextWord();
        word = takeNextWord();
        while(!wordIsCard(word, "INPUT") && !configFile.eof()){

          if(word=="STAGE"){
            word=takeNextWord();
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            if(word=="pre_guess"){
              stage=0;
            }
            else if(word=="post_calc"){
              stage=1;
            }
            else{
              error = NEWTON_ERROR;
              checkError(error, "ERROR. Bad mapper card STAGE. Select \"pre_guess\" or \" post_calc\" - Parser::parseInput");
            }
            word=takeNextWord();
          }

          else if(word=="MAP_CLIENT"){
            clientName = takeNextWord();
            word = takeNextWord();
          }

          else if(word=="GUESSES_MAPPED"){
            word = takeNextWord();
            stringstream(word) >> nPostMap;
            word = takeNextWord();
          }

          else if(word=="CALCS_PRE_MAP"){
            word = takeNextWord();
            stringstream(word) >> nPreMap;
            word = takeNextWord();
          }

          else if(word=="MATERIAL"){
            mapper->nMat++;
            word = takeNextWord();
            while(!wordIsCard(word, "MAPPER") && 
                  !wordIsCard(word, "INPUT") && 
                  !configFile.eof()){
              word = takeNextWord();
            }
          }

          else if(word=="ENERGY_PER_FISSION"){
            word = takeNextWord();
            while(!wordIsCard(word, "MAPPER") && 
                  !wordIsCard(word, "INPUT") && 
                  !configFile.eof()){
              word = takeNextWord();
            }
          }

          else if(word=="MAP_DATA"){
            mapper->xsFile = takeNextWord();
            word = takeNextWord();
          }

          else if(word=="DATA_N_GROUPS"){
            word = takeNextWord();
            stringstream(word) >> mapper->nGroups;
            word = takeNextWord();

            // Looking for the client that corresponds:
            for(int iF=0; iF<client->nFermi; iF++){
              if(client->fermi[iF].name==clientName){
                if(mapper->nGroups != client->fermi[iF].nGroups){
                  rootPrints("WARNING: Groups used in \""+clientName+"\" are different from data loaded - Parser::parseInput");
                }
              }
            }            
          }

          else if(!wordIsForbidden(word)){
            error = NEWTON_ERROR;
            checkError(error, "ERROR. Bad mapper configuration. Word: \""+word+"\" unknown - Parser::parseInput");
          }

          else{
            word = takeNextWord();
          }

        }

        // Checking consistency of data readed
        if (stage==0 && nPostMap<0){
          error = NEWTON_ERROR;
          checkError(error, "ERROR. Bad mapper configuration. Set GUESSES_MAPPED using STAGE: PRE_GUESS - Parser::parseInput");

        }
        else if(stage==1 && nPreMap<0) {
          error = NEWTON_ERROR;
          checkError(error, "ERROR. Bad mapper configuration. Set CALS_PRE_MAP using STAGE: POST_CALC - Parser::parseInput");
        }
        if(clientName==""){
          error = NEWTON_ERROR;
          checkError(error, "ERROR. Set MAP_CLIENT card - Parser::parseInput");
        }

        // Saving data
        for(int iCode=0; iCode<sys->nCodes; iCode++){
          if(sys->code[iCode].name==clientName){
            if(stage==0){
              sys->code[iCode].gammaMap = mapName;
              sys->code[iCode].nDelta = nPostMap;
            }
            else if(stage==1){
              sys->code[iCode].alphaMap = mapName;
              sys->code[iCode].nAlpha = nPreMap;
            }
            break;
          }
        }

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
  
  // TEST
/*  cout<<"MAPPERS"<<endl;
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    if(sys->code[iCode].gammaMap!=""){
      cout<<sys->code[iCode].name<<": "<<sys->code[iCode].gammaMap<<endl;
      cout<<"number of guesses mapped: "<<sys->code[iCode].nDelta<<endl;
    }
    if(sys->code[iCode].alphaMap!=""){
      cout<<sys->code[iCode].name<<": "<<sys->code[iCode].alphaMap<<endl;
      cout<<"number of calcs pre map: "<<sys->code[iCode].nAlpha<<endl;
    }
  }*/

  // Allocate things

  mapper->allocate1();

  
  // MAPPER SPECIFIC READINGS
  int materialReaded = 0;

  // Eighth input file opening  

  configFile.open("newton.config");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Count things in mapper
      if(word=="MAPPER"){
        word = takeNextWord();
        while(!wordIsCard(word, "INPUT") && !configFile.eof()){

          if(word=="ENERGY_PER_FISSION"){
            for(int iE=0; iE<mapper->nGroups; iE++){
              word = takeNextWord();
              if(!wordIsForbidden(word)){
                stringstream(word)>>mapper->energyPerFission[iE];
              }
              else{
                error = NEWTON_ERROR;
                checkError(error, "Error setting ENERGY_PER_FISSION card - Parser::parseInput");
              }
            }
            word = takeNextWord();
            if(!wordIsForbidden(word)){
              rootPrints("WARNING: Setting extra groups in ENERGY_PER_FISSION card? - Parser::parseInput");
            }
          }

          else if(word=="MATERIAL"){
            mapper->mat[materialReaded].name = takeNextName();            
            word = takeNextWord();            
            while(wordIsCard(word, "MATERIAL")){
              if(word=="N_BURNUP"){
                word = takeNextWord();
                stringstream(word)>>mapper->mat[materialReaded].nBurnupPoints;
                word = takeNextWord();
              }
              else if(word=="PHYS_ENT"){                
                // Set material in physical entities structures in fermi client
                word = takeNextWord();
                while(!wordIsForbidden(word) && !configFile.eof()){
                  // Look for this physical entity in fermi clients
                  for(int iF=0; iF<client->nFermi;iF++){
                    for(int ipe=0; ipe<client->fermi[iF].nPhysicalEntities;ipe++){
                      if(word==client->fermi[iF].pe[ipe].name){
                        client->fermi[iF].pe[ipe].material = materialReaded;
                      }
                    }
                  }

                  word = takeNextWord();
                }
              }

            }
            materialReaded++;
          }

          else{
            word = takeNextWord();  
          }

        }
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
  
 // TEST
/* cout<<"MAPPER SPECIFICS"<<endl;
 for(int iF=0; iF<client->nFermi; iF++){
  cout<<client->fermi[iF].name<<endl;
  for(int ipe=0; ipe<client->fermi[iF].nPhysicalEntities;ipe++){
    int imat = client->fermi[iF].pe[ipe].material;
    cout<<client->fermi[iF].pe[ipe].name<<" - Material: "<< mapper->mat[imat].name<<" - n Burnup points to load: "<<mapper->mat[imat].nBurnupPoints<<endl;
  }
  cout<<"Energy per fision per groups:"<<endl;
  for(int ig=0; ig<mapper->nGroups; ig++){
    cout<<mapper->energyPerFission[ig]<<" ";
  }
 }
 exit(1); */ 
  
}

/* Parser::checkConsistencyInPhases

Checks if there are not repeated clients between phases.

input: System pointer
output: -

*/
void Parser::checkConsistencyInPhases(System* sys)
{
/*  string iClient, jClient;
  for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
    for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
      iClient = sys->codeToConnectInPhase[iPhase][iCode];
      for(int jPhase=0; jPhase<sys->nPhasesPerIter; jPhase++){
        for(int jCode=0; jCode<sys->nCodesInPhase[jPhase]; jCode++){
          jClient = sys->codeToConnectInPhase[jPhase][jCode];
          if(! (iPhase==jPhase && iCode==jCode)){
            if(iClient==jClient){
              error = NEWTON_ERROR;
              checkError(error, "Bad formulation. Check card PHASES - Parser::checkConsistencyInPhases");
            }
          }
        }
      }
    }    
  }*/
  
}

/* Parser::isAComment

Analyze if the string is a comment.

input: string
output: true if it is a comment, negative other case

*/

bool Parser::isAComment(string word)
{
  if( word=="NEWTON" ||
      word[0]=='#' ||
      word[0]=='@' ||
      word[0]=='%' ||
      word[0]=='_' ){
    return true;
  }
  return false;
}
