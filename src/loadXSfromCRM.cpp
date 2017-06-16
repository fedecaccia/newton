/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	MAPPER
calculations			    |
                      |

-------------------------------------------------------------------------------

Load cross sections and dependences from CRM file.

Author: Federico A. Caccia
Date: 16 June 2017

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
  crm.open(xsFile.c_str());
  if (crm.is_open()){
    
  }
  else{
   error = NEWTON_ERROR;
   checkError(error,"Error opening \""+xsFile+"\"");
  }
  crm.close();  
  
}
