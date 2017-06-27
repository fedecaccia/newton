/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | SYSTEM
maste code            |
                      |

-------------------------------------------------------------------------------

System contains all the usefull information of the strucure of the problem, 
like number of unknowns, connectivities, etc.

Date: 9 June 2017

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

#include "System.h"

using namespace::std;

/* System constructor
*/
System::System()
{
  // Number of client codes
  nCodes = 0;
  // Number of unknowns (equal to residuals and betas)
  nUnk = 0;
  // Number of residuals (equal to unknowns and betas)
  nRes = 0;
  // Number of betas (equal to unknowns and residuals)
  nBeta = 0;
  // Number of gammas
  nGamma = 0;
  // Number of phases in explicit serial method
  nPhasesPerIter = 0;
  // Math object
  math = new MathLib();
  
  // Initial error value
	error = NEWTON_SUCCESS;
}


/* System::allocate1
Allocates main variables of the system.
Set number of phases(rows) in wich codes ID(col) run in each phase 
(in explicit serial method).
After each phase, guesses are updated with previous calculations.
Also, set number of arguments that each bin has to take.
Called by Parser.

input: method integer
output: -

*/
void System::allocate1(int method)
{ 
  // Client data
  code = new client[nCodes];
  for(int  iCode=0; iCode<nCodes; iCode++){
      code[iCode].name = "";
      code[iCode].connection = NEWTON_UNKNOWN_CONNECTION;
      code[iCode].nProcs = 1;
      code[iCode].nArgs = 0;
      code[iCode].nAlpha = 0;
      //code[iCode].alpha = new
      code[iCode].nBeta = 0;
      code[iCode].betaFirstValuePos = 0;
      code[iCode].nGamma = 0;
      code[iCode].gammaFirstValuePos = 0;
      code[iCode].nDelta = 0;
      //code[iCode].delta = new
      code[iCode].alphaMap = "";
      code[iCode].gammaMap = "";
      
      code[iCode].inputModelName = "";
      code[iCode].inputModel = "";
      code[iCode].actualInputName = "";      
      code[iCode].actualInput = "";
      code[iCode].inputExt = "";
      code[iCode].restartName = "";
      code[iCode].actualRestartName = "";
      code[iCode].actualRestart = "";
      code[iCode].restartExt = "";
      code[iCode].restartPath = "";
      code[iCode].outputName = "";
      code[iCode].actualOutputName = "";
      code[iCode].actualOutput = "";
      code[iCode].outputExt = "";
      code[iCode].outputPath = "";
      code[iCode].binCommand = "";
      code[iCode].commandToRun = "";
    
  }
  
  // Method is not EXPLICI_SERIAL
  if(method!=EXPLICIT_SERIAL){
    nPhasesPerIter = 1;
  }  
  
  // Number of codes per phase
  nCodesInPhase = new int[nPhasesPerIter];

  // Code ID (col) to connect in each phase (row)
  codeToConnectInPhase = new string*[nPhasesPerIter];
  // Initialization
  for(int iPhase=0; iPhase<nPhasesPerIter; iPhase++){
    nCodesInPhase[iPhase] = 0;
  } 
  if(method!=EXPLICIT_SERIAL){
    nCodesInPhase[0] = nCodes;
  }
  
  /* Allocate unknowns as residuals, and residuals names.
   If unknows set by input are different, 
   * then parser puts NEWTON_ERROR. */
  xName = new string[nRes];
  resName = new string[nRes];
  // Initialization
  for(int iRes=0; iRes<nRes; iRes++){
    xName[iRes] = "";    
    resName[iRes] = "";    
  }
  
  // Links
  x2linkInRes = new int[nRes];
  beta2linkInRes = new int[nRes];
  beta2linkInX = new int[nRes];
  // Secure initialization
  for(int iRes =0; iRes<nRes; iRes++){
    x2linkInRes[iRes]= -1;
    beta2linkInRes[iRes]= -1;
    beta2linkInX[iRes] = -1;
  }

}


/* System::allocate3

Allocate number of codes (col) in each phase in codeToConnectInPhase2.

input: -
output: -

*/

void System::allocate3()
{ 
  
  // Number of arguments of each client code
  
  for(int iCode=0; iCode<nCodes; iCode++){
    code[iCode].arg = new string[code[iCode].nArgs];
    // Initialization
    for(int iArg=0; iArg<code[iCode].nArgs; iArg++){
      code[iCode].arg[iArg] = "";
    }
  }

  // System variables
  
  beta = new double[nBeta];
  betaName = new string[nBeta];
  gamma = new double[nGamma];
  gammaName = new string[nGamma];
  x2linkInGamma = new int[nGamma];
  beta2linkInGamma = new int[nGamma];

  // Initialization
  for(int iBeta=0; iBeta<nBeta; iBeta++){
    beta[iBeta] = 0;    
    betaName[iBeta]="";    
  }
  
  for(int iGamma=0; iGamma<nGamma; iGamma++){
    gamma[iGamma] = 0;
    gammaName[iGamma]="";
    // Secure initialization
    x2linkInGamma[iGamma] = -1;
    beta2linkInGamma[iGamma] = -1;
  }


  // Code name (col) to connect in each phase (row)
  for(int iPhase=0; iPhase<nPhasesPerIter; iPhase++){
    codeToConnectInPhase[iPhase] = new string[nCodesInPhase[iPhase]];
    for(int iCode=0; iCode<nCodesInPhase[iPhase]; iCode++){
      if(nPhasesPerIter>1){
        codeToConnectInPhase[iPhase][iCode] = "";
      }
      else{
        codeToConnectInPhase[0][iCode] = code[iCode].name;
      }
    }
  }
  
}

/* System::construct
Sets the configuration of the system.

input: -
output: -

*/
void System::construct(Mapper* mapper)
{
  
  rootPrints("Building system...");
  
  // Allocate alphas and deltas in each code
  // Alphas and deltas could had been updated in the last parsing,
  // reading mappers
  
  // Alphas and Deltas
  for(int iCode=0; iCode<nCodes; iCode++){
    code[iCode].alpha = new double[code[iCode].nAlpha];
    code[iCode].delta = new double[code[iCode].nDelta];
    math->zeros(code[iCode].alpha, code[iCode].nAlpha);
    math->zeros(code[iCode].delta, code[iCode].nDelta);
  }
    
  // Construct the correct links
  
  // Beta to link in residuals
  for(int iRes=0; iRes<nRes; iRes++){
    bool resLinked = false;
    for(int iBeta=0; iBeta<nBeta; iBeta++){
      if(betaName[iBeta]==resName[iRes]){
        beta2linkInRes[iRes] = iBeta;
        resLinked = true;
      }
    }
    if(resLinked == false){
      error = NEWTON_ERROR;
      checkError(error, "Bad beta names - System::construct");
    }
  }
  
  // x to link in residuals
  for(int iRes=0; iRes<nRes; iRes++){
    bool xLinked = false;
    for(int iX=0; iX<nUnk; iX++){
      if(xName[iX]==resName[iRes]){
        x2linkInRes[iRes] = iX;
        xLinked = true;
      }
    }
    if(xLinked == false){
      error = NEWTON_ERROR;
      checkError(error, "Bad x names - System::construct");
    }
  } 
  
  // Beta to link in gamma
  for(int iGamma=0; iGamma<nGamma; iGamma++){
    bool gammaLinked = false;
    for(int iBeta=0; iBeta<nBeta; iBeta++){
      if(betaName[iBeta]==gammaName[iGamma]){
        beta2linkInGamma[iGamma] = iBeta;
        gammaLinked = true;
      }
    }
    if(gammaLinked == false){
      error = NEWTON_ERROR;
      checkError(error, "Bad beta or gamma names - System::construct");
    }
  }

  // Beta to link in X
  for(int iX=0; iX<nUnk; iX++){
    bool xLinked = false;
    for(int iBeta=0; iBeta<nBeta; iBeta++){
      if(betaName[iBeta]==xName[iX]){
        beta2linkInX[iX] = iBeta;
        xLinked = true;
      }
    }
    if(xLinked == false){
      error = NEWTON_ERROR;
      checkError(error, "Bad beta or x names - System::construct");
    }
  }

  // x to link in gamma
  for(int iGamma=0; iGamma<nGamma; iGamma++){
    bool gammaLinked = false;
    for(int iX=0; iX<nUnk; iX++){
      if(xName[iX]==gammaName[iGamma]){
        x2linkInGamma[iGamma] = iX;
        gammaLinked = true;
      }
    }
    if(gammaLinked == false){
      error = NEWTON_ERROR;
      checkError(error, "Bad x or gamma names - System::construct");
    }
  }
  
  // Set position of first values of local betas and gammas in global
  int betaPos = 0;
  int gammaPos = 0;
  for (int iCode=0; iCode<nCodes; iCode++){
    code[iCode].betaFirstValuePos = betaPos;
    betaPos += code[iCode].nBeta;
    code[iCode].gammaFirstValuePos = gammaPos;
    gammaPos += code[iCode].nGamma;
  }
  /*
  // TEST
  cout<<"b2linkInRes  -  x2linkInRes"<<endl;
  for(int iRes=0; iRes<nRes; iRes++){
    cout<<beta2linkInRes[iRes]<<"\t\t"<<x2linkInRes[iRes]<<endl;
  }
  
  cout<<"b2linkInx"<<endl;
  for(int iRes=0; iRes<nRes; iRes++){
    cout<<beta2linkInX[iRes]<<endl;
  }
  exit(1);
  */ 
  
  // Initialize file names and command args in I/O case
  error = setFilesAndCommands(0);
  
  // Mapper object
  NewtonMap=mapper;
  
  // List of maps
  nMap = 0;
  listOfMaps = new string[nCodes*2];
  string auxMap;
  bool mapSaved;
  for(int iCode=0; iCode<nCodes; iCode++){
    auxMap = code[iCode].alphaMap;
    mapSaved = false;
    if(auxMap!="" && auxMap !="none"){
      for(int iMap=0; iMap<nMap; iMap++){
        if(auxMap==listOfMaps[iMap]){
          mapSaved = true;
          break;
        }
      }
      if(!mapSaved){
        listOfMaps[nMap]=auxMap;
        nMap++;
      }
    }
    auxMap = code[iCode].gammaMap;
    mapSaved = false;
    if(auxMap!="" && auxMap !="none"){
      for(int iMap=0; iMap<nMap; iMap++){
        if(auxMap==listOfMaps[iMap]){
          mapSaved = true;
          break;
        }
      }
      if(!mapSaved){
        listOfMaps[nMap]=auxMap;
        nMap++;
      }
    }
  }
  //~ // TEST
  //~ for(int iMap=0; iMap<nMap; iMap++){
    //~ cout<<listOfMaps[iMap]<<endl;
  //~ }
  
  // Configurate mappers
  for(int iMap=0; iMap<nMap; iMap++){
    NewtonMap->config(listOfMaps[iMap]);
  }
  
	checkError(error,"Error setting system structure.");
}

/* System::setFilesAndCommands

Set input, output, restart file and command to run strings.
This only matters to codes wich are connected by I/O.

input: step integer
output: error

*/
int System::setFilesAndCommands(int step)
{
  
  for(int iCode=0; iCode<nCodes; iCode++){
    // Input file
    code[iCode].inputModel = code[iCode].inputModelName+"."+code[iCode].inputExt;
    code[iCode].actualInputName = code[iCode].inputModelName+"_step"+int2str(step);
    code[iCode].actualInput = code[iCode].actualInputName+"."+code[iCode].inputExt;
    // Ouput file
    if(code[iCode].outputName=="FROM_INPUT" || 
       code[iCode].outputName=="from_input" || 
       code[iCode].outputName=="From_input" ||
       code[iCode].outputName==""){
      code[iCode].actualOutputName = code[iCode].actualInputName;
      code[iCode].actualOutput = code[iCode].actualOutputName+"."+code[iCode].outputExt;
    }
    else{
      code[iCode].actualOutputName = code[iCode].outputName;
      code[iCode].actualOutput = code[iCode].actualOutputName+"."+code[iCode].outputExt;
    }
    // Restart file
    if(code[iCode].restartName=="FROM_INPUT" || 
       code[iCode].restartName=="from_input" || 
       code[iCode].restartName=="From_input" ||
       code[iCode].restartName==""){
      code[iCode].actualRestartName = code[iCode].actualInputName;
      code[iCode].actualRestart = code[iCode].actualRestartName+"."+code[iCode].restartExt;
    }
    else{
      code[iCode].actualRestartName = code[iCode].restartName;
      code[iCode].actualRestart = code[iCode].actualRestartName+"."+code[iCode].restartExt;
    }
    // Command to run
    code[iCode].commandToRun = code[iCode].binCommand;
    for(int iArg=0; iArg<code[iCode].nArgs; iArg++){
      // Code ID
      if(code[iCode].arg[iArg]=="code_id" || 
         code[iCode].arg[iArg]=="CODE_ID" ||
         code[iCode].arg[iArg]=="Code_id"){
        transform(code[iCode].arg[iArg].begin(), code[iCode].arg[iArg].end(), code[iCode].arg[iArg].begin(), ::tolower);
        code[iCode].commandToRun +=" "+int2str(code[iCode].id);
      }
      // Actual input with extension
      else if(code[iCode].arg[iArg]=="input" || code[iCode].arg[iArg]=="input_file" ||
              code[iCode].arg[iArg]=="INPUT" || code[iCode].arg[iArg]=="INPUT_FILE" ||
              code[iCode].arg[iArg]=="Input" || code[iCode].arg[iArg]=="Input_file" ){
        transform(code[iCode].arg[iArg].begin(), code[iCode].arg[iArg].end(), code[iCode].arg[iArg].begin(), ::tolower);
        code[iCode].commandToRun +=" "+code[iCode].actualInput;
      }
      // Actual input without extension
      else if(code[iCode].arg[iArg]=="input_name" || 
              code[iCode].arg[iArg]=="INPUT_NAME" ||
              code[iCode].arg[iArg]=="Input_name"){
        transform(code[iCode].arg[iArg].begin(), code[iCode].arg[iArg].end(), code[iCode].arg[iArg].begin(), ::tolower);
        code[iCode].commandToRun +=" "+code[iCode].actualInputName;
      }
      // Actual output with extension
      else if(code[iCode].arg[iArg]=="output" || 
              code[iCode].arg[iArg]=="OUTPUT" ||
              code[iCode].arg[iArg]=="Output"){
        transform(code[iCode].arg[iArg].begin(), code[iCode].arg[iArg].end(), code[iCode].arg[iArg].begin(), ::tolower);
        code[iCode].commandToRun +=" "+code[iCode].actualOutput;
      }
      // Actual output without extension
      else if(code[iCode].arg[iArg]=="output_name" || 
              code[iCode].arg[iArg]=="OUTPUT_NAME" ||
              code[iCode].arg[iArg]=="Output_name"){
        transform(code[iCode].arg[iArg].begin(), code[iCode].arg[iArg].end(), code[iCode].arg[iArg].begin(), ::tolower);
        code[iCode].commandToRun +=" "+code[iCode].actualOutputName;
      }
      // Else, just use the string readed as argument
      else{
        code[iCode].commandToRun += " "+code[iCode].arg[iArg];
      }
    }
  }  
  return error;
}


void System::alpha2beta(int iCode)
{
  error= NewtonMap->map(iCode, code[iCode].name, code[iCode].alphaMap, code[iCode].nAlpha, code[iCode].alpha, code[iCode].nBeta, &(beta[code[iCode].betaFirstValuePos]));
  checkError(error, "Error building beta - System::alpha2beta");
}


void System::computeResiduals(double* res, double* x)
{
  for(int iRes=0; iRes<nRes; iRes++){
    res[iRes] = -x[x2linkInRes[iRes]] + beta[beta2linkInRes[iRes]];
  }
}

/* System::computePhaseResiduals
Compute module of residual in actual phase iteration.
It is useful using EXPLICIT_SERIAL method, with multiple iterations inside phases.

NOT IMPLEMENTED YET. WE SHOULD COMPUTE DIFFERENCE BETWEEN GAMMAS AND BETAS OF RIGHT CODES.

input: phase number, pointer to the residual
output: -
*/
void System::computePhaseResiduals(int phase, double* res)
{
  *res=1;
}


void System::x2gamma(double* x)
{  
  for(int iGamma=0; iGamma<nGamma; iGamma++){
    gamma[iGamma] = x[x2linkInGamma[iGamma]];
    //cout<<gamma[iGamma]<<endl;
  }
}


void System::beta2x(double* x)
{  
  for (int iX=0; iX<nUnk; iX++){
    x[iX] = beta[beta2linkInX[iX]];
  }
}


void System::beta2gamma()
{
  for(int iGamma=0; iGamma<nGamma; iGamma++){
    gamma[iGamma] = beta[beta2linkInGamma[iGamma]];
  }
}


void System::gamma2delta(int iCode)
{
  error= NewtonMap->map(iCode, code[iCode].name, code[iCode].gammaMap, code[iCode].nGamma, &(gamma[code[iCode].gammaFirstValuePos]), code[iCode].nDelta, code[iCode].delta);
  checkError(error, "Error building delta from gamma - System::gamma2delta");
}

/* System::findCodeInPhase

Returns the code number that run in phase.

input: numebr of phase, number of code in phase
output: integer code number

*/
int System::findCodeInPhase(int iPhase, int iPhaseCode)
{
  string codeName = codeToConnectInPhase[iPhase][iPhaseCode];
  int iCode=-1;
  for(int jCode=0; jCode<nCodes; jCode++){
    if(code[jCode].name==codeName){
      iCode = jCode;
    }
  }
  if(iCode==-1){
    error = NEWTON_ERROR;
    checkError(error, "Bad name for code in phase: \""+codeName+"\" - System::findCodeInPhase");
  }

  return iCode;
}
