/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | MAPPER
maste code            |
                      |

-------------------------------------------------------------------------------

Mapper manages the preprocessing and / or postprocessing of the variables 
received and / or sended to the clients.

Date: 21 June 2017

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

/*Mapper::th2xs
 
Map vector of thermal-hydraulic values into another of cross sections.

input: code, number of elements to map, vector to map, 
number of elements of image, image vector, & pointer to the current fermi 
struct
output: error

*/
int Mapper::th2xs(int nXToMap, double* xToMap, int nMapped, double* mapped, Client::fermiStruct* fermi)
{
  // Organizing data
  
  double* tRef = new double [fermi->nPhysicalEntities];
  double* tFuel = new double [fermi->nPhysicalEntities];
  double* nRef = new double [fermi->nPhysicalEntities];
  double* burnup = new double [fermi->nPhysicalEntities];
  
  /* Assuming that in xToMap values are:
   * tRef, tFuel, nRef, (phys ent 1)
   * tRef, tFuel, nRef, (phys ent 2),
   * ...
  (this is how I set in newton.config)*/
  
  int ivalue=0;
  for (int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    tRef[ipe] = xToMap[ivalue];
    tFuel[ipe] = xToMap[ivalue+1];
    nRef[ipe] = xToMap[ivalue+2];
    ivalue+=3;
    burnup[ipe] = fermi->pe[ipe].burnup;
  }  


  // XS with dependencies, interpolated betwen burnup points.
  double*** xs = new double** [fermi->nPhysicalEntities];
  // XS with dependencies, of previous burnup point.
  double*** xsDown = new double** [fermi->nPhysicalEntities];
  // XS with dependencies, of posterior burnup point.
  double*** xsUp = new double** [fermi->nPhysicalEntities];
  // XS condended in groups
  double*** xsCondensed = new double** [fermi->nPhysicalEntities];
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    xs[ipe] = new double*[nXSTypes]; 
    xsDown[ipe] = new double*[nXSTypes];
    xsUp[ipe] = new double*[nXSTypes];
    for(int ixs=0; ixs<nXSTypes; ixs++){
      xs[ipe][ixs] = new double[nGroups];
      xsDown[ipe][ixs] = new double[nGroups];
      xsUp[ipe][ixs] = new double[nGroups];
    }
    xsCondensed[ipe] = new double*[fermi->nXS];
    for(int ixs=0; ixs<fermi->nXS; ixs++){
      xsCondensed[ipe][ixs] = new double[fermi->nGroups]; 
    }
  }

  // Interpolating with crm data
  
  // Looking for loaded previous and posterior burnup points
  
  int* downBP = new int[fermi->nPhysicalEntities];
  int* upBP = new int[fermi->nPhysicalEntities];
  
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    int iMat = fermi->pe[ipe].material;
    for(int ibp=0; ibp<mat[iMat].nBurnupPoints-1; ibp++){
      if(burnup[ipe]<=mat[iMat].burnup[ibp+1]){
        for(int ixs=0; ixs<nXSTypes; ixs++){
          for(int ig=0; ig<nGroups; ig++){
            xsDown[ipe][ixs][ig] = mat[iMat].xsRef[ibp][ixs][ig];
            xsUp[ipe][ixs][ig] = mat[iMat].xsRef[ibp+1][ixs][ig];
          }          
        }
        downBP[ipe] = ibp;
        upBP[ipe] = ibp+1;
        break;
      }
    }
    int lastbp = mat[iMat].nBurnupPoints-1;
    if(burnup[ipe] >= mat[iMat].burnup[lastbp]){
      if(burnup[ipe] > mat[iMat].burnup[lastbp]){
        rootPrints("WARNING: Not enough burnup points readed");
        rootPrints("Physical entity: \""+fermi->pe[ipe].name+" \" has burnup: "+dou2str(burnup[ipe]));
        rootPrints("Using last point readed, with burnup: "+dou2str(mat[iMat].burnup[lastbp]));
        rootPrints("Increase amount of burnup points to load from CRM file");      
      }
      downBP[ipe] = lastbp;
      upBP[ipe] = lastbp;
      for(int ixs=0; ixs<nXSTypes; ixs++){
        for(int ig=0; ig<nGroups; ig++){
          xsDown[ipe][ixs][ig] = mat[iMat].xsRef[lastbp][ixs][ig];
          xsUp[ipe][ixs][ig] = mat[iMat].xsRef[lastbp][ixs][ig];
        }          
      }
    }
  }

  // Calculating deltas in known burnup points
  
  double depVal=0;
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    int iMat = fermi->pe[ipe].material;
    for(int ixs=0; ixs<nXSTypes; ixs++){
      for(int ig=0; ig<nGroups; ig++){
        for(int iDep=0; iDep<mat[iMat].nDependencies; iDep++){
          // Looking for dependency. This names have to be loaded reading data file
          if(mat[iMat].depName[iDep] == "fuelTemp"){
            depVal = tFuel[ipe];
          }
          else if(mat[iMat].depName[iDep] == "refTemp"){
            depVal = tRef[ipe];
          }
          else if(mat[iMat].depName[iDep] == "refDens"){
            depVal = nRef[ipe];
          }
          else{
            error = NEWTON_ERROR;
            checkError(error, "Bad dependency number - Mapper::th2xs");
          }
          xsDown[ipe][ixs][ig] += calculateDeltaXS(iMat, downBP[ipe], ixs, ig, iDep, depVal);
          xsUp[ipe][ixs][ig]   += calculateDeltaXS(iMat,   upBP[ipe], ixs, ig, iDep, depVal);
        }
      }
    }
  }
  
  //~ // TEST
  //~ for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    //~ cout<<fermi->pe[ipe].name<<" - burnup: "<<fermi->pe[ipe].burnup<<endl;
    //~ cout<<"XS bottom:"<<endl;
    //~ for(int ixs=0; ixs<nXSTypes; ixs++){
      //~ for(int ig=0; ig<nGroups; ig++){
        //~ cout<<xsDown[ipe][ixs][ig]<<" ";
      //~ }
      //~ cout<<endl;
    //~ }
    //~ cout<<"XS up:"<<endl;
    //~ for(int ixs=0; ixs<nXSTypes; ixs++){
      //~ for(int ig=0; ig<nGroups; ig++){
        //~ cout<<xsUp[ipe][ixs][ig]<<" ";
      //~ }
      //~ cout<<endl;
    //~ }
  //~ }
  //~ exit(1);
  
  // Interpolating between burnup points
  
  // xs_i = xsDown_i + (xsUp_i - xsDown_i)/deltaBurnup * iDeltaBurnup  
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    int iMat = fermi->pe[ipe].material;
    double deltaBurnup = mat[iMat].burnup[upBP[ipe]] - mat[iMat].burnup[downBP[ipe]];
    double iDeltaBurnup = burnup[ipe] - mat[iMat].burnup[downBP[ipe]];
    double deltaFactor = 0;
    if(deltaBurnup>0){
      deltaFactor = iDeltaBurnup / deltaBurnup;
    }
    double deltaXS;
    for(int ixs=0; ixs<nXSTypes; ixs++){
      for(int ig=0; ig<nGroups; ig++){
        deltaXS = xsUp[ipe][ixs][ig] - xsDown[ipe][ixs][ig];
        xs[ipe][ixs][ig] = xsDown[ipe][ixs][ig] + deltaXS * deltaFactor;
      }
    }
  }

  // TEST
  //~ for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    //~ cout<<fermi->pe[ipe].name<<" - burnup: "<<fermi->pe[ipe].burnup<<endl;
    //~ cout<<"XS interpolated:"<<endl;
    //~ for(int ixs=0; ixs<nXSTypes; ixs++){
      //~ for(int ig=0; ig<nGroups; ig++){
        //~ cout<<xs[ipe][ixs][ig]<<" ";
      //~ }
      //~ cout<<endl;
    //~ }
  //~ }
  //~ exit(1);  
  
  // Condensing groups in necessary case
  
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){  
    if(fermi->nGroups==1){ // Just adding values, so invalid!
      // Diffusion coefficient
      xsCondensed[ipe][0][0] = 0;
      for(int ig=0; ig<nGroups; ig++){
        xsCondensed[ipe][0][0] += xs[ipe][0][ig];
      }
      // XS absortion
      xsCondensed[ipe][1][0] = 0;
      for(int ig=0; ig<nGroups; ig++){
        xsCondensed[ipe][1][0] += xs[ipe][1+ig][ig];
      }
      // nu XF
      xsCondensed[ipe][2][0] = 0;
      for(int ig=0; ig<nGroups; ig++){
        xsCondensed[ipe][2][0] += xs[ipe][1+nGroups][ig];
      }
      // E*XF
      xsCondensed[ipe][3][0] = 0;
      for(int ig=0; ig<nGroups; ig++){
        xsCondensed[ipe][3][0] += energyPerFission[ig] * xs[ipe][2+nGroups][ig];
      }
    }
    else if(fermi->nGroups==nGroups){ // Not condensing necessity
      for(int ixs=0; ixs<nXSTypes; ixs++){
        for(int ig=0; ig<nGroups; ig++){
          xsCondensed[ipe][ixs][ig] = xs[ipe][ixs][ig];
        }
      }
    }
    else{
      error = NEWTON_ERROR;
      checkError(error, "I don't know how to condense groups - Mapper::th2xs");
    }
  }
  
  /* Map values in the order that are going to be send to the client.
  * We are setting "deltas" to the client here. */
  ivalue=0;
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    for(int ixs=0; ixs<fermi->nXS; ixs++){
      for(int ig=0; ig<fermi->nGroups; ig++){
        mapped[ivalue] = xsCondensed[ipe][ixs][ig];
        ivalue++;
      }
    } 
  }
  
  return error;
}


/* Mapper::deltaXS
Calculate delta XS for a particular dependency.

input: material, burnup point, xs type, xs group, dependency type, dependency value
output: delta XS

*/
double Mapper::calculateDeltaXS(int iMat, int burnupPoint, int ixs, int ig, int iDep, double depVal)
{
  // Dependency reference value
  depRef = mat[iMat].refDepVal[iDep];
  //~ cout<<"DEP ref:"<<depRef<<endl;
  //~ cout<<"DEP val:"<<depVal<<endl;
  // Delta dependency value
  deltaInDependency = depVal - depRef;
 
  // XS initialized in zero
  deltaXS = 0;
  
  // Other variables
  
  // Materials with more than just one point of dependency
  if(mat[iMat].nPointsPerDependency[iDep]>1){
    // Browsing deltas in dependencies to found the right interval
    for(int iDelta=0; iDelta<mat[iMat].nPointsPerDependency[iDep]-1; iDelta++){
      // If belongs to this interval
      if( ((deltaInDependency <= mat[iMat].deltaDepVal[iDep][iDelta]) && 
           (deltaInDependency >= mat[iMat].deltaDepVal[iDep][iDelta+1]))
           // or if it is beyond the extremes
        || (deltaInDependency > mat[iMat].deltaDepVal[iDep][0])
        || (deltaInDependency < mat[iMat].deltaDepVal[iDep][mat[iMat].nPointsPerDependency[iDep]-1])){
        // If both deltas have same sign
        if(mat[iMat].deltaDepVal[iDep][iDelta] * mat[iMat].deltaDepVal[iDep][iDelta+1] > 0){
          // delta y of interval
          deltaXSi = mat[iMat].xs[burnupPoint][iDep][iDelta][ixs][ig] - mat[iMat].xs[burnupPoint][iDep][iDelta+1][ixs][ig];
          // delta x of interval
          deltaDepi = mat[iMat].deltaDepVal[iDep][iDelta] - mat[iMat].deltaDepVal[iDep][iDelta+1];
          // change in x inside interval
          deltaDepVali = deltaInDependency - mat[iMat].deltaDepVal[iDep][iDelta+1];        
          // Adding delta at previous point
          deltaXS = mat[iMat].xs[burnupPoint][iDep][iDelta+1][ixs][ig];
        }
        // if deltas change signs (they are around 0)
        else {           
          // delta over reference
          if(deltaInDependency>0){          
            // delta y of interval
            deltaXSi = mat[iMat].xs[burnupPoint][iDep][iDelta][ixs][ig];
            // delta x of interval
            deltaDepi = mat[iMat].deltaDepVal[iDep][iDelta];
            // change in x inside interval
            deltaDepVali = deltaInDependency;
          }
          // delta below reference
          else{          
            // delta y of interval
            deltaXSi = -(mat[iMat].xs[burnupPoint][iDep][iDelta+1][ixs][ig]);
            // delta x of interval
            deltaDepi = -(mat[iMat].deltaDepVal[iDep][iDelta+1]);
            // change in x inside interval
            deltaDepVali = deltaInDependency;
          }
        }
        // change in y = slope * change in x
        deltaXS += deltaXSi / deltaDepi * deltaDepVali;
      }
    }
  }
  // Just one delta
  else{
    // delta y of interval
    deltaXSi = mat[iMat].xs[burnupPoint][iDep][0][ixs][ig];
    // delta x of interval
    deltaDepi = mat[iMat].deltaDepVal[iDep][0];
    /// change in x inside interval
    deltaDepVali = deltaInDependency;
    
    // change in y = slope * change in x
    deltaXS += deltaXSi / deltaDepi * deltaDepVali;    
  }  
  
  return deltaXS;
}
