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

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <iomanip>

#include "global.h"

class Debugger
{
  public:
    Debugger();
    void setOutput(std::string*, int n=1);
    void setOn(int o=0);
    void setOff(int o=0);
    void log(std::string, int o=0, int width=20);
    void allLog(std::string, int o=0);
    void finish();
    
  private:

    int error;
    int nLogs;
    bool* debugIsOn;    
    std::string* logName;
    std::ofstream* logFile;
};


#endif
