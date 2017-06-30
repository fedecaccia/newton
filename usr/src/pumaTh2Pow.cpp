/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | CLIENT
maste code            |
                      |

-------------------------------------------------------------------------------

In this class user can programm specific routines to read output and write
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

#include "Client.h"

using namespace::std;

/* Client::preparePumaInput
 
In this function user can programm all necessary to write an input
for an specific client code.
Values in prepare(...)input are in the same order than the corresponding
mapper has saved them. If there are not mapper, values are in the same order 
that has been set in GUESSES in "newton.config".

input: code name, amount of values to send, vector with values to send 
and inputs file string (to write, and to read as model)
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::preparePumaInput(string codeName, int nValues, double* values, string input, string inputModel)
{ 
  // Input strings are dummies because I need to write 3 files  
  
  // Zones  
  int nFE = 61;
  int nAxialZones = 28;
  int thPar = 3;
  
  // Check consistency
  if(nFE*nAxialZones*thPar != nValues){
    cout<<"Error. Bad amount of values received: "<<nValues<<"; "<<nFE*nAxialZones*thPar<<" expected - Client::preparePumaInput";
  }
    
  // This deppends on how I order values in mapper
  
  /* Values received: 
   * T_cool_Channel1_z1
   * T_cool_Channel2_z1
   * ..
   * T_cool_ChannelN_z1
   * T_cool_Channel1_z2
   * ..
   * T_cool_ChannelN_z2
   * ..
   * T_cool_ChannelN_zN
   
   * T_fuel_Channel1_z1
   * T_fuel_Channel2_z1
   * ..
   * T_fuel_ChannelN_z1
   * T_fuel_Channel1_z2
   * ..
   * T_fuel_ChannelN_z2
   * ..
   * T_fuel_ChannelN_zN
   
   * N_cool_Channel1_z1
   * N_cool_Channel2_z1
   * ...
   * N_cool_ChannelN_z1
   * N_cool_Channel1_z2
   * ...
   * N_cool_ChannelN_z2
   * ...
   * N_cool_ChannelN_zN */

  
  // Input file 1
  inputFile.open("coolantTemp.dat");
  inputFile.precision(numeric_limits<double>::digits10 + 1);
	if (inputFile.is_open()){
    for(int i=0; i<nAxialZones; i++){
      for(int j=0; j<nFE; j++){
        inputFile << values[i*nFE+j]<<" ";
      }
      inputFile<<endl;
    }
	}
	else{
		cout<<"Error writing input file for code: "<<codeName<<" - Client::preparePumaInput"<<endl;
    error = NEWTON_ERROR;
	}
  inputFile.close();
  
  
  // Input file 2
  inputFile.open("fuelTemp.dat");
  inputFile.precision(numeric_limits<double>::digits10 + 1);
	if (inputFile.is_open()){
    for(int i=nAxialZones; i<2*nAxialZones; i++){
      for(int j=0; j<nFE; j++){
        inputFile << values[i*nFE+j]<<" ";
      }
      inputFile<<endl;
    }
	}
	else{
		cout<<"Error writing input file for code: "<<codeName<<" - Client::preparePumaInput"<<endl;
    error = NEWTON_ERROR;
	}
  inputFile.close();
  
  
  // Input file 3
  inputFile.open("coolantDensity.dat");
  inputFile.precision(numeric_limits<double>::digits10 + 1);
	if (inputFile.is_open()){
    for(int i=2*nAxialZones; i<3*nAxialZones; i++){
      for(int j=0; j<nFE; j++){
        inputFile << values[i*nFE+j]<<" ";
      }
      inputFile<<endl;
    }
	}
	else{
		cout<<"Error writing input file for code: "<<codeName<<" - Client::preparePumaInput"<<endl;
    error = NEWTON_ERROR;
	}
  inputFile.close();
 
 return error; 
}

/* Client::readPumaOutput
 
In this function user can programm all necessary to read an output
for an specific client code.
Values in read(...)output has to be saved in the same order than the 
corresponding mapper uses them. If there are not mapper, values has to be saved
in the same order that has been set in CALCS in "newton.config".

input: code name, amount of values to read, vector with values to read 
and output file string
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::readPumaOutput(string codeName, int nValues, double* values, string output)
{
  // Zones  
  int nFE = 61;
  int nAxialZones = 28;
  
  // Check consistency
  if(nFE*nAxialZones != nValues){
    cout<<"Error. Bad amount of values to read: "<<nValues<<"; "<<nFE*nAxialZones<<" expected - Client::readPumaOutput"<<endl;
  }
  
  /* Values readed:
   * Pow_Channel1_z1
   * Pow_Channel2_z1
   * ...
   * Pow_ChannelN_z1
   * Pow_Channel1_z2
   * ...
   * Pow_ChannelN_z2
   * ...
   * Pow_ChannelN_zN */
  
  // Output file  
  outputFile.open(output.c_str());
	if (outputFile.is_open()){
		for(int i=0; i<nValues; i++){
      outputFile >> values[i];
    }
	}
	else{
		cout<<"Error reading output file from code: "<<codeName<<" - Client::readPumaOutput"<<endl;
    error = NEWTON_ERROR;
	}
  outputFile.close();  
 
 return error; 
}
