/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CONTOL ROD
coupling              | MANAGER
maste code            |
                      |

-------------------------------------------------------------------------------

This programm is a client code made for manager control rod positions.

Date: 23 June 2017

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

#ifndef CONTROL_ROD_H
#define CONTROL_ROD_H

#define CR_SUCCESS  					    0
#define CR_ERROR  			 			    1

#define POW4K2CR1                 0

#define CONTINUE                  0
#define RESTART                   1
#define FINISH                    2
#define ABORT                     3

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <limits>
#include <string>
#include <stdlib.h>
#include <mpi.h>

std::string int2str (int);
double* loaddata(std::string, int);
void printResults(double*, int, std::string);
void mpi_connection();
void mpi_receive(double*, int);
int mpi_receive_order();
void mpi_send(double*, int);
void mpi_finish();
void mpi_split_and_comm();
void mpi_free();

class controlRod
{
	public:
		controlRod();
		void solve();

	private:
    double* input;
    double* output;
    double desiredK;
    double actualK;
    double* crPos;
    std::string file;
    std::string fileInput;
    std::string fileOutput;		
};

#endif