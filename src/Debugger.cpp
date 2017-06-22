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
  // Default initialization
  debugIsOn=false;
  logName = "";
  
  // Initialize error
  error = NEWTON_SUCCESS;
}

/* Debugger::setOutput
Set log file name.

input: string log file name
output: -

*/
void Debugger::setOutput(string fileName)
{
  logName = fileName;
}

/* Debugger::setOn
Switch to on debug action.

input: -
output: -

*/
void Debugger::setOn()
{
  debugIsOn=true;
  if (!logFile.is_open()){
    if(logName!=""){
      logFile.open(logName.c_str());
    }
    else{
      rootPrints("WARNING: Trying to set on debugger but log file name has not been provided - Debugger::setOn");
    }
  }
}

/* Debugger::setOff
Switch to off debug action.

input: -
output: -

*/
void Debugger::setOff()
{
  debugIsOn=false;
  if (logFile.is_open()){
    logFile.close();
  }
}

/* Debugger::log
Only root prints some string into output.

input: string
output: -

*/
void Debugger::log(string logStr)
{
  if (logFile.is_open()){
    if(irank==NEWTON_ROOT){
      logFile << logStr <<endl;
    }
  }
}

/* Debugger::allLog
All processes that call allLog try to print some string into output.
WARNING: Competitive process.

input: string
output: -

*/
void Debugger::allLog(string logStr)
{
  cout<<irank<<endl;
  if (logFile.is_open()){
    logFile << "(irank: "<<int2str(irank)<<") - "<< logStr <<endl;
  }
}

/* Debugger::finish
Close log file if it has been opened.

input: -
output: -

*/
void Debugger::finish()
{
  debugIsOn = false;
  if (logFile.is_open()){
    logFile.close();
  }
}
