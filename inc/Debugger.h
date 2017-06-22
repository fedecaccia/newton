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

#include "global.h"

class Debugger
{
  public:
    Debugger(int); // cantidad de arvhivos de salida
    void setOutput(std::string*); //nombres
    void setOn(int*); // recibe vector con 1 y 0 para prender
    void setOff(int*); // recibe vector con 1 y 0 para apagar
    void log(int, std::string); //int salida
    void allLog(int, std::string);
    void finish();
    
  private: 
    int error;
    bool* debugIsOn;    
    std::string* logName;
    std::ofstream* logFile;
};


#endif
