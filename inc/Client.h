/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | CLIENT
maste code            |
                      |

-------------------------------------------------------------------------------

In this class the user can programm specific routines to read output and write
inputs for a particular client code.

Date: 7 June 2017

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

#ifndef CLIENT_H
#define CLIENT_H

#include "global.h"

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <limits>
#include <string>

class Client
{
	public:
		Client();
    void allocate1();
    void allocate2();
    void updateVars(double);
    int prepareInput(int, std::string, int, double*, std::string, std::string);
    int readOutput(int, std::string, int, double*, std::string);
    
    int nFermi;
    struct fermiStruct{
      std::string name;
      int nPhysicalEntities;
      int nGroups;
      int nXS;
      struct physicalEntity{
        std::string name;
        int material;
        double burnup;
        double** xs;
        double* EFissionRate;
      };
      physicalEntity* pe;
    };
    fermiStruct* fermi;
    
    int nRelap;
    struct relapStruct{
      std::string name;
      int nAxialZones;
      int nRadialPoints;
      double t0;
      double tf;
      double t;
      double maxDt;
      std::string* hs;
      std::string* pipe;
      double* fpow;
      double voidf;
      double voidg;
      double tempf;
      double tempg;
      double* tempRef;
      double* rhomix;
      double tempFuelC;
      double tempFuelV;
      double* tempDoppFuel;
    };
    relapStruct* relap;
		
	private:
  
    int prepareTestClientInput(std::string, int, double*, std::string, std::string);
    int readTestClientOutput(std::string, int, double*, std::string);
    
    int prepareUserClientInput(std::string, int, double*, std::string, std::string);
    int readUserClientOutput(std::string, int, double*, std::string);
    
    int prepareRelapPow2thInput(std::string, int, double*, std::string, std::string);
    int readRelapPow2thOutput(std::string, int, double*, std::string);
    
    int prepareFermiXs2powInput(std::string, int, double*, std::string, std::string);
    int readFermiXs2powOutput(std::string, int, double*, std::string);
    
    std::string takeNextWord();
    std::string takeNextLine();
    
		int error;
    std::string word;
    std::string line;
    std::ofstream inputFile;
    std::ifstream inputModelFile;
    std::ifstream outputFile;

    int nValuesExpected;
};

#endif
