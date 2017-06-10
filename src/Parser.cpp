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
    if(word=="STEPS_JAC_CALC"){
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
    if(word=="GUESSES"){
      return true;
    }
    if(word=="CALCS"){
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
  }
  else if(parent=="GUESSES"){
    if(word=="MAP"){
      return true;
    }
    if(word=="GUESSES_MAPPED"){
      return true;
    }
  } 
  else if(parent=="CALCS"){
    if(word=="MAP"){
      return true;
    }
    if(word=="CALCS_PRE_MAP"){
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
  while (isAComment(word)){          
    getline(configFile, word);
    // Next word
    configFile >> word;
    //transform(word.begin(), word.end(), word.begin(), ::tolower);
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
     wordIsCard(word, "CALCS") || 
     wordIsCard(word, "GUESSES") ||
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
  
  while(wordIsCard(word, "CALCS")){
    if(word=="MAP"){
      word = takeNextWord();
      if(!wordIsForbidden(word)){
        sys->code2[clientReaded].alphaMap = word;
      }
      else{
        error = NEWTON_ERROR;
        checkError(error, "\""+word+"\" word is forbidden to use as mapper - Parser::loadCalcsAndTakeWord");
      }
      word = takeNextWord();
    }
    else if(word=="CALCS_PRE_MAP"){
      word = takeNextWord();
      stringstream(word)>>sys->code2[clientReaded].nAlpha;
      word = takeNextWord();
    }
  }

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
  
  while(wordIsCard(word, "GUESSES")){
    if(word=="MAP"){
      word = takeNextWord();
      if(!wordIsForbidden(word)){
        sys->code2[clientReaded].alphaMap = word;
      }
      else{
        error = NEWTON_ERROR;
        checkError(error, "\""+word+"\" word is forbidden to use as mapper - Parser::loadCalcsAndTakeWord");
      }
      word = takeNextWord();
    }
    else if(word=="GUESSES_MAPPED"){
      word = takeNextWord();
      stringstream(word)>>sys->code2[clientReaded].nDelta;
      word = takeNextWord();
    }
  }

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
    sys->code2[clientReaded].name = word;
    word = takeNextWord();
  }
  else{
    error = NEWTON_ERROR;
    checkError(error, "Select a correct name for client. Word "+word+" is avoided - Parser::loadClientAndTakeWord");
  }
  
  // Take properties

  while(wordIsCard(word, "CLIENT") && !configFile.eof()){

    if(word=="CALCS"){
      word = loadCalcsAndTakeWord(sys);
    }
    
    else if(word=="GUESSES"){
      word = loadGuessesAndTakeWord(sys);
    }
    
    else if(word=="INPUT_NAME"){
      word = takeNextWord();
      sys->code2[clientReaded].inputModelName = word;
      word = takeNextWord();
    }
    
    else if(word=="INPUT_EXT"){
      word = takeNextWord();
      sys->code2[clientReaded].inputExt = word;
      word = takeNextWord();
    }
    
    else if(word=="RESTART_NAME"){
      word = takeNextWord();
      sys->code2[clientReaded].restartName = word;
      word = takeNextWord();
    }
    
    else if(word=="RESTART_EXT"){
      word = takeNextWord();
      sys->code2[clientReaded].restartExt = word;
      word = takeNextWord();
    }
    
    else if(word=="RESTART_PATH"){
      word = takeNextWord();
      sys->code2[clientReaded].restartPath = word;
      word = takeNextWord();
    }
    
    else if(word=="OUTPUT_NAME"){
      word = takeNextWord();
      sys->code2[clientReaded].outputName = word;
      word = takeNextWord();
    }
    
    else if(word=="OUTPUT_EXT"){
      word = takeNextWord();
      sys->code2[clientReaded].outputExt = word;
      word = takeNextWord();
    }
    
    else if(word=="OUTPUT_PATH"){
      word = takeNextWord();
      sys->code2[clientReaded].outputPath = word;
      word = takeNextWord();
    }
    
    else if(word=="BIN_COMMAND"){
      word = takeNextWord();
      sys->code2[clientReaded].binCommand = word;
      word = takeNextWord();
    }
    
    else if(word=="CONNECTION"){
      word = takeNextWord();
      transform(word.begin(), word.end(), word.begin(), ::toupper);

      if(word=="MPI"){
        sys->code2[clientReaded].connection = NEWTON_MPI_COMMUNICATION;
      }
      else if(word=="IO"){
        sys->code2[clientReaded].connection = NEWTON_SPAWN;
      }
      else if(word=="PPLEP"){
        sys->code2[clientReaded].connection = NEWTON_PPLEP;
      }
      else{
        error = NEWTON_ERROR;
        checkError(error, "Unknown connection type: "+word+" - Parser::loadClientAndTakeWord");
      }
      word = takeNextWord();
    }
    
    else if(word=="ARGS"){
      word = takeNextWord();
      do{
        if(!wordIsForbidden(word)){
          sys->code2[clientReaded].nArgs++;
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
      stringstream(word) >> sys->code2[clientReaded].nProcs;
      word = takeNextWord();
    }

    else{
      word = takeNextWord();
    }
    
  } // Finish while(word inside client)

  // Bad cases

  /*if(!wordIsForbidden(word)){
    error = NEWTON_ERROR;
    checkError(error, "Unknown input word: \""+word+"\" - Parser::parseInput");
  }*/

  // Bad position of guesses and calcs properties

  if(wordIsCard(word, "GUESSES") ||
     wordIsCard(word, "CALCS")){
    error = NEWTON_ERROR;
    checkError(error, "Bad place for input card: \""+word+"\" - Parser::loadClientAndTakeWord");
  }
  
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

  if(sys->code2[client].connection==NEWTON_UNKNOWN_CONNECTION){
    error = NEWTON_ERROR;
    checkError(error, "CONNECTION unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
  }

  // I/O connection type

  if(sys->code2[client].connection==NEWTON_SPAWN){
    if(sys->code2[client].inputModelName==""){
      error = NEWTON_ERROR;
      checkError(error, "INPUT_NAME unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
    }
    if(sys->code2[client].inputExt==""){
      error = NEWTON_ERROR;
      checkError(error, "INPUT_EXT unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
    }
    if(sys->code2[client].binCommand==""){
      error = NEWTON_ERROR;
      checkError(error, "BIN_COMMAND unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
    }
  }

  // Variables
    if(sys->code2[client].nAlpha==0){
      error = NEWTON_ERROR;
      checkError(error, "CALCS_PRE_MAP unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
    }
    if(sys->code2[client].nBeta==0){
      error = NEWTON_ERROR;
      checkError(error, "CALCS unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
    }
    if(sys->code2[client].nGamma==0){
      error = NEWTON_ERROR;
      checkError(error, "GUESSES unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
    }
    if(sys->code2[client].nDelta==0){
      error = NEWTON_ERROR;
      checkError(error, "GUESSES_MAPPED unknown in client: "+sys->code2[client].name+ " - Parser::checkClientProperties");
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
      
      if(word=="CLIENT"){
        sys->nCodes++;
        word = takeNextWord();
        // Client name
        word = takeNextWord();
      }

      // Parsing numerical methods
      
      else if(word=="N_STEPS"){
        word = takeNextWord();
        stringstream(word) >> evol->nSteps;
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
      
      else if(word=="METHOD"){
        word = takeNextWord();
        transform(word.begin(), word.end(), word.begin(), ::toupper);
        if(word=="EXPLICIT_SERIAL"){
          sol->method = EXPLICIT_SERIAL;
        }
        else if(word=="EXPLICIT_PARALLEL"){
          sol->method = EXPLICIT_SERIAL;
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
  
  sys->allocate1(); // ********************************************change things
  sys->allocate2(); // ****************************************************** change names

  // Second input file opening
  
  configFile.open("newton.configtest");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Count amount of codes that run in each phase in EXPLICIT_SERIAL
      
      if(word=="PHASES"){
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
      
      // Analyze all properties of the client and return the word out of that

      else if(word=="CLIENT"){        
        word = loadClientAndTakeWord(sys);        
      }
      
      // Properties in bad order
      else if(wordIsCard(word, "CLIENT")){
        error = NEWTON_ERROR;
        checkError(error, "Word in bad place. Set "+word+" after CLIENT - Parser::parseInput");
      }
      else if(wordIsCard(word, "CALCS") || wordIsCard(word, "GUESSES")){
        error = NEWTON_ERROR;
        checkError(error, "Set CALCS and GUESSES properties after list variables. Word: "+word+" in bad place - Parser::parseInput");
      }
    
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

  // TEST
  // cout<<"Number of clients: "<<sys->nCodes<<endl;
  // cout<<"Number of unknowns: "<<sys->nUnk<<" - Number of residuals: "<<sys->nRes<<endl;
  // cout<<"Unknowns order: "<<endl;
  // for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
  //   cout<<sys->xName[iUnk]<<endl;
  // }
  // cout<<"Nonlinear method: "<<sol->method<<endl;
  // cout<<"Phases: "<<sys->nPhasesPerIter<<endl;
  // for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
  //   cout<<" "<<iPhase<<"- n codes in phase: "<<sys->nCodesInPhase[iPhase]<<endl;
  // }
  // cout<<"Clients: "<<endl;
  // for(int iCode=0; iCode<sys->nCodes; iCode++){
  //   cout<<"Code: "<<sys->code2[iCode].name<<endl;
  //   cout<<" - nAlpha: "<<sys->code2[iCode].nAlpha<<endl;
  //   cout<<" - nBeta: "<<sys->code2[iCode].nBeta<<endl;
  //   cout<<" - nGamma: "<<sys->code2[iCode].nGamma<<endl;
  //   cout<<" - nDelta: "<<sys->code2[iCode].nDelta<<endl;
  //   cout<<" - alphaMap: "<<sys->code2[iCode].alphaMap<<endl;
  //   cout<<" - gammaMap: "<<sys->code2[iCode].gammaMap<<endl;
  //   cout<<" - n Procs: "<<sys->code2[iCode].nProcs<<endl;
  //   cout<<" - n Args: "<<sys->code2[iCode].nArgs<<endl;
  //   cout<<" - connection type: "<<sys->code2[iCode].connection<<endl;
  //   cout<<" - command to run bin: "<<sys->code2[iCode].binCommand<<endl;
  //   cout<<" - Input model name: "<<sys->code2[iCode].inputModelName<<endl;
  //   cout<<" - Input extension: "<<sys->code2[iCode].inputExt<<endl;
  //   cout<<" - Restart name: "<<sys->code2[iCode].restartName<<endl;
  //   cout<<" - Restart extension: "<<sys->code2[iCode].restartExt<<endl;
  //   cout<<" - Output model name: "<<sys->code2[iCode].outputName<<endl;
  //   cout<<" - Output extension: "<<sys->code2[iCode].outputExt<<endl;
  // }

  // Reinitialization
  betaLoaded = 0;
  gammaLoaded = 0;
  clientReaded =0;

  // Third input file opening  

  configFile.open("newton.configtest");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Read clients that run in each phase
      
      if(word=="PHASES"){
        for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
            word = takeNextWord();
            sys->codeToConnectInPhase2[iPhase][iCode] = word;
          }
          // Take "&"
          word = takeNextWord();
        }
        word = takeNextWord();
      }      
      
      // Read variables

      else if(word=="CLIENT"){
        for(int loading=0; loading<2; loading++){
          while(word!="CALCS" && word!= "GUESSES"){
            word = takeNextWord();
          }
          if(word=="CALCS"){
            for(int iBeta=0; iBeta<sys->code2[clientReaded].nBeta; iBeta++){
              word = takeNextWord();
              sys->betaName[betaLoaded] = word;
              betaLoaded++;
            }
          }
          else if(word=="GUESSES"){
            for(int iGamma=0; iGamma<sys->code2[clientReaded].nGamma; iGamma++){
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

  configFile.open("newton.configtest");
  if (configFile.is_open()){
    while(!configFile.eof()){

      // Read args
      if(word=="CLIENT"){
        if(sys->code2[clientReaded].nArgs>0){          
          while(word!="ARGS"){
            word = takeNextWord();
          }
          if(word=="ARGS"){
            for(int iArg=0; iArg<sys->code2[clientReaded].nArgs; iArg++){
              word = takeNextWord();
              sys->code2[clientReaded].arg[iArg] = word;
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

  cout<<"Codes to connect in each phase in EXPLICIT_SERIAL"<<endl;
  for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
    for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
      cout<<" - "<<sys->codeToConnectInPhase2[iPhase][iCode]<<" ";
    }
    cout<<endl;
  }

  cout<<"Global betas: "<<sys->nBeta<<endl;
  for(int iBeta=0; iBeta<sys->nBeta; iBeta++){
    cout<<" - "<<sys->betaName[iBeta]<<": "<<sys->beta[iBeta]<<endl;
  }
  cout<<"Global gammas: "<<sys->nGamma<<endl;
  for(int iGamma=0; iGamma<sys->nGamma; iGamma++){
    cout<<" - "<<sys->gammaName[iGamma]<<": "<<sys->gamma[iGamma]<<endl;
  }

  cout<<" Arguments"<<endl;
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    cout<<" ("<<sys->code2[iCode].nArgs<<"): ";
    for(int iArg=0; iArg<sys->code2[iCode].nArgs; iArg++){
      cout<<sys->code2[iCode].arg[iArg]<<" ";
    }
    cout<<endl;
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
            //stringstream(word) >> sys->codeToConnectInPhase2[iPhase][iCode];         
            configFile >> word;
          }
          configFile >> word;
        }
        
        // TEST
        //~ for (int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
          //~ for(int iCode=0; iCode<sys->nCodesInPhase[iPhase]; iCode++){
            //~ cout<< sys->codeToConnectInPhase2[iPhase][iCode]<<" ";
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
  if( word=="NEWTON" ||
      word[0]=='#' ||
      word[0]=='@' ||
      word[0]=='%' ||
      word[0]=='_' ||
      word[0]=='-'){
    return true;
  }
  return false;
}
