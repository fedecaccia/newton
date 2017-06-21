/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | MAPPER
maste code            |
                      |

-------------------------------------------------------------------------------

Load cross sections and dependences from CRM file.

Date: 16 June 2017

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

#include "Mapper.h"

using namespace::std;

/* Mapper::loadXSfromCRM

Read xs data file.

input: -
output: -

*/
void Mapper::loadXSfromCRM()
{
  //rootPrints("Loading XS from crm...");
  crm.open(xsFile.c_str());
  if (crm.is_open()){
    while(!crm.eof()){
      word = takeNextLine();
      if(word=="* DEFINIR TABLAS"){
        word = takeNextWord();
        int auxGroup;
        stringstream(word) >> auxGroup;
        if(auxGroup!=nGroups){
          error = NEWTON_ERROR;
          checkError(error, "XS file has data in: "+word+" groups");
        }
        if(nGroups==5){
          nXSTypes = 8;
        }
        else{
          error = NEWTON_ERROR;
          checkError(error, "Set amount o XS types for "+int2str(nGroups)+" groups - Mapper::loadXSfromCRM");
        }
        break;
      }
    }

    while(!crm.eof()){
      word = takeNextWord();
      
      if(word=="Material"){
        word = takeNextLine();
        for(int iMat=0; iMat<nMat; iMat++){
          if(mat[iMat].name==word){
            //rootPrints("Reading: "+word);
            materialReaded = iMat;
            break;
          }
        }        
      }

      if(word=="dependencia" && materialReaded>=0){

        word = takeNextWord();
        word = takeNextWord();
        if(word == "1,"){
          mat[materialReaded].nDependencies = 3;
          mat[materialReaded].nPointsPerDependency = new int[3];
          mat[materialReaded].refDepVal = new double[3];          
          mat[materialReaded].deltaDepVal = new double*[3]; 
          mat[materialReaded].depName = new string[3];
        }
        else{
          error = NEWTON_ERROR;
          checkError(error, "Unknown dependency type in crm file - Mapper::loadXSfromCRM");
        }
      }

      // Reading dependencies
      if(word=="bloques:" && materialReaded>=0){
        // Only dependency material
        if(mat[materialReaded].nDependencies!=0){

          for(int iDep=0; iDep<mat[materialReaded].nDependencies; iDep++){

            word = takeNextWord();
            if(word=="'TEMPERATURA"){
              word = takeNextWord();
              word = takeNextWord();
              if(word=="COMBUSTIBLE'"){                
                mat[materialReaded].depName[iDep] = "fuelTemp";
              }
              else if(word=="REFRIGERANTE'"){
                mat[materialReaded].depName[iDep] = "refTemp";
              }
            }
            else if(word=="'DENSIDAD"){
              word = takeNextWord();
              word = takeNextWord();
              mat[materialReaded].depName[iDep] = "refDens";
            }
            word = takeNextWord();
            word = takeNextWord();
            word = takeNextWord();
            stringstream(word) >> mat[materialReaded].refDepVal[iDep];
            word = takeNextWord();
            word = takeNextWord();
            if(word=="41"){
              mat[materialReaded].nPointsPerDependency[iDep] = 1;
              mat[materialReaded].deltaDepVal[iDep] = new double[1];
            }
            else if(word=="82"){
              mat[materialReaded].nPointsPerDependency[iDep] = 2;
              mat[materialReaded].deltaDepVal[iDep] = new double[2];
            }
            else if(word=="123"){
              mat[materialReaded].nPointsPerDependency[iDep] = 3;
              mat[materialReaded].deltaDepVal[iDep] = new double[3];
            }
            // Complete reading line
            word = takeNextLine();
          }
        }

      } // end looking into 'bloques'

      // Loading xs
      if(word=="tabla:"){

        // If valid material
        if(materialReaded>=0){
          // Allocating burnup points for materials
          mat[materialReaded].burnup = new double[mat[materialReaded].nBurnupPoints];

          // Allocating reference xs
          mat[materialReaded].xsRef = new double**[mat[materialReaded].nBurnupPoints];
          for(int iBP=0; iBP<mat[materialReaded].nBurnupPoints; iBP++){
            mat[materialReaded].xsRef[iBP] = new double*[nXSTypes];
            for(int iXS=0; iXS<nXSTypes; iXS++){
              mat[materialReaded].xsRef[iBP][iXS] = new double[nGroups];
            }
          }

          // Allocating delta xs
          if(mat[materialReaded].nDependencies>0){
            mat[materialReaded].xs = new double****[mat[materialReaded].nBurnupPoints];
            for(int iB=0; iB<mat[materialReaded].nBurnupPoints; iB++){
              mat[materialReaded].xs[iB] = new double***[mat[materialReaded].nDependencies];
              for(int iDep=0; iDep<mat[materialReaded].nDependencies; iDep++){
                mat[materialReaded].xs[iB][iDep] = new double**[mat[materialReaded].nPointsPerDependency[iDep]];
                for(int iDepP=0; iDepP<mat[materialReaded].nPointsPerDependency[iDep]; iDepP++){
                  mat[materialReaded].xs[iB][iDep][iDepP] = new double*[nXSTypes];
                  for(int iXS=0; iXS<nXSTypes; iXS++){
                    mat[materialReaded].xs[iB][iDep][iDepP][iXS] = new double[nGroups];
                  }
                }
              }
            }
          }

          // Burnup points loop
          for(int iBurnupPoint=0; iBurnupPoint<mat[materialReaded].nBurnupPoints; iBurnupPoint++){
            // Reference point for burnup
            word = takeNextWord();
            stringstream(word) >> mat[materialReaded].burnup[iBurnupPoint];
            for(int ixs=0; ixs<nXSTypes; ixs++){
              for(int ig=0; ig<nGroups; ig++){
                word = takeNextWord();
                stringstream(word) >> mat[materialReaded].xsRef[iBurnupPoint][ixs][ig];
              }
            }
            // Dependences
            for(int iDep=0; iDep<mat[materialReaded].nDependencies; iDep++){
              for(int iDepP=0; iDepP<mat[materialReaded].nPointsPerDependency[iDep]; iDepP++){
                word = takeNextWord();
                stringstream(word) >> mat[materialReaded].deltaDepVal[iDep][iDepP];
                for(int ixs=0; ixs<nXSTypes; ixs++){
                  for(int ig=0; ig<nGroups; ig++){
                    word = takeNextWord();
                    stringstream(word) >> mat[materialReaded].xs[iBurnupPoint][iDep][iDepP][ixs][ig];
                  }
                }
              }
            }

          }
        }

        // Setting non valid material until next interested founded
        materialReaded = -1;
      }

    }

  }
  else{
   error = NEWTON_ERROR;
   checkError(error,"Error opening \""+xsFile+"\"");
  }
  crm.close();


  // TEST
/*  for(int iMat=0; iMat<nMat; iMat++){
    cout<<mat[iMat].name<<endl;
    cout<<"n burnup points: "<<mat[iMat].nBurnupPoints<<endl;
    cout<<"n dependencies: "<<mat[iMat].nDependencies<<endl;
    for(int iDep=0; iDep<mat[iMat].nDependencies; iDep++){
      cout<<"Dependency: "<<mat[iMat].depName[iDep]<<endl;
      cout<<" dependency ref val: "<<mat[iMat].refDepVal[iDep]<<endl;
      cout<<" n points per dependency: "<<mat[iMat].nPointsPerDependency[iDep]<<endl;
      for(int ip=0; ip<mat[iMat].nPointsPerDependency[iDep]; ip++){
        cout<<"  dep delta val: "<<mat[iMat].deltaDepVal[iDep][ip]<<endl;
      }
    }
    cout<<"burnup values:"<<endl;
    for(int iBP=0; iBP<mat[iMat].nBurnupPoints; iBP++){
      cout<<mat[iMat].burnup[iBP]<<endl;
    }

    cout<<"XS ref:"<<endl;
    for(int iBP=0; iBP<mat[iMat].nBurnupPoints; iBP++){
      cout<<"Burnup value: "<<mat[iMat].burnup[iBP]<<endl;
      for(int ixs=0; ixs<nXSTypes; ixs++){
        for(int ig=0; ig<nGroups; ig++){
          cout<<mat[iMat].xsRef[iBP][ixs][ig]<< " ";
        }
        cout<<endl;
      }
    }

    cout<<"delta XS:"<<endl;
    for(int iBP=0; iBP<mat[iMat].nBurnupPoints; iBP++){
      cout<<"Burnup value: "<<mat[iMat].burnup[iBP]<<endl;
      for(int iDep=0; iDep<mat[iMat].nDependencies; iDep++){
        cout<<"dependency: "<<iDep<<endl;
        for(int ip=0; ip<mat[iMat].nPointsPerDependency[iDep]; ip++){
          cout<< "dependency point: "<<ip<<endl;
          for(int ixs=0; ixs<nXSTypes; ixs++){
            for(int ig=0; ig<nGroups; ig++){
              cout<<mat[iMat].xs[iBP][iDep][ip][ixs][ig]<< " ";
            }
            cout<<endl;
          }
        }
      }
    }

  }
  exit(1);*/
  
}

/* Mapper::takeNextWord
Get the next word.

input: -
output: word string

*/
string Mapper::takeNextWord()
{
  // Next word
  crm >> word;

  return word;
}

/* Mapper::takeNextLine
Get the next line.

input: -
output: word string

*/
string Mapper::takeNextLine()
{
  // Next word
  getline(crm, word);
  // Erase blank spaces at first and end
  for(unsigned int i=0; i<word.length(); i++){
    if(word[0]==' '){
      word.erase(word.begin());
    }
    else{
      break;
    }
  }
  for(unsigned int i=0; i<word.length(); i++){
      for(unsigned int i=0; i<word.length(); i++){
    if(*word.rbegin()==' '){
      word.erase(word.end()-1);
    }
    else{
      break;
    }
  }
  if(*word.rbegin()=='.'){
      word.erase(word.end()-1);
    }
    else{
      break;
    }
  }

  return word;
}

