/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | TEST
coupling              | HEADER
maste code            |
                      |

-------------------------------------------------------------------------------

This programm is a client code made for test Newton functionalities.
args:

Date: 17 June 2017

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

#ifndef TEST_H
#define TEST_H

#define TEST_SUCCESS 					    0
#define TEST_ERROR			 			    1
    
#define TEST_2_LINEAR 				    0
#define TEST_3_LINEAR 				    1
#define TEST_2_NONLINEAR 			    2
#define TEST_3_NONLINEAR          3
#define TEST_2_LINEAR_MAPPER      4

#define CONTINUE                  0
#define RESTART                   1
#define FINISH                    2
#define ABORT                     3

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <limits>
#include <string>
#include <mpi.h>
#include <math.h>

std::string int2str (int);
double* loaddata(std::string, int);
void printResults(double*, int, std::string);
void mpi_connection();
void mpi_receive(double*, int);
int mpi_receive_order();
void mpi_send(double*, int);
void mpi_disconnect();
void mpi_split_and_comm();
void mpi_free();
void mpi_finalize();

class linear2
{
	public:
		linear2();
		void solve();

	private:
    double* input;
    double* output;
    double w,x,y,z;
    double** mat;
    double* b;
    std::string file;
    std::string fileInput;
    std::string fileOutput;
		
};

class linear3
{
	public:
		linear3();
		void solve();

	private:
    double* input;
    double* output;
    double w,x,y,z;
    double** mat;
    double* b;
    std::string file;
    std::string fileInput;
    std::string fileOutput;
};

class nonlinear2
{
	public:
		nonlinear2();
		void solve();

	private:
    double* input;
    double* output;    
    std::string file;
    std::string fileInput;
    std::string fileOutput;

    double a, b, l, m, x, y;

};

class nonlinear3
{
  public:
    nonlinear3();
    void solve();

  private:
    double* input;
    double* output;    
    std::string file;
    std::string fileInput;
    std::string fileOutput;

    double a, b, c, l, m, n, x, y, z;

};

class linear2mapper
{
  public:
    linear2mapper();
    void solve();

  private:
    double* input;
    double* output;
    double w,x,y,z, alpha;
    double** mat;
    double* b;
    std::string file;
    std::string fileInput;
    std::string fileOutput;
    
};



#endif
