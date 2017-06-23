/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | DEBUGGER
maste code            |
                      |

-------------------------------------------------------------------------------

Debugger is the class that manages all the specific logs of the code. 

Date: 22 June 2017

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
along with sNewton.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#include "Debugger.h"

using namespace::std;

/* Debugger constructor
*/
Debugger::Debugger()
{
  // Initialize error
  error = NEWTON_SUCCESS;
}

/* Debugger::setOutput
Set log file name, allocate, etc.

input: amount of file names, vector of string log file names
output: -

*/
void Debugger::setOutput(string* fileName, int n)
{
  // Initialization
  nLogs = n;
  debugIsOn = new bool[nLogs];
  logName = new string[nLogs];
  logFile = new ofstream[nLogs];
  for(int i=0; i<nLogs; i++){
    debugIsOn[i] = false;
    logName[i] = fileName[i];
  }
}

/* Debugger::setOn
Switch to on debug action in specific log file.

input: number of file, (default 0)
output: -

*/
void Debugger::setOn(int o)
{
  if(o<nLogs){
    debugIsOn[o] = true;
    if (!logFile[o].is_open()){
      logFile[o].open(logName[o].c_str());
    }
  }
  else{
    error = NEWTON_ERROR;
    checkError(error, "Trying to set on bad log file - Debugger::setOutput");
  }
}

/* Debugger::setOff
Switch to off debug action in specific log file.

input: number of file, (default 0)
output: -

*/
void Debugger::setOff(int o)
{
  if(o<nLogs){
    debugIsOn[o] = false; 
    if (logFile[o].is_open()){
      logFile[o].close();
    }
  }
  else{    
    error = NEWTON_ERROR;
    checkError(error, "Trying to set off bad log file - Debugger::setOutput");
  }
}

/* Debugger::log
Only root prints some string into output.

input: string, output file, width between words (default:20)
output: -

*/
void Debugger::log(string logStr, int output, int width)
{
  if(output<nLogs){
    if (logFile[output].is_open()){
      if(irank==NEWTON_ROOT){
        logFile[output] << setw(width) << logStr <<flush;
      }
    }
  }
  else{
    error = NEWTON_ERROR;
    checkError(error, "ERROR. Trying to print in a non valid output - Debugger::log");
  }
}

/* Debugger::allLog
All processes that call allLog try to print some string into output.
WARNING: Competitive process.

input: string
output: -

*/
void Debugger::allLog(string logStr, int output)
{
  if(output<nLogs){
    if (logFile[output].is_open()){
      if(irank==NEWTON_ROOT){
        logFile[output] << "(irank: "<<int2str(irank)<<") - "<< logStr << endl;
      }
    }
  }
  else{
    error = NEWTON_ERROR;
    checkError(error, "ERROR. Trying to print in a non valid output - Debugger::log");
  }
}

/* Debugger::finish
Close log files if they have been opened.

input: -
output: -

*/
void Debugger::finish()
{
  for(int i=0; i<nLogs; i++){
    debugIsOn[i] = false;
    if (logFile[i].is_open()){
      logFile[i]<<endl;
      logFile[i].close();
    }
  }
}
