#undef __FUNCT__
#define __FUNCT__ "main"

int main(int argc,char **argv)
{  
  
  NewtonFeatures newt();
  newt.init() // MPI
  
  ParserClass* Parser = new ParserClass();
  error = Parser->parseFile();checkError();
  
  SystemClass* System = new SystemClass();
  error = System->initialize();checkError(); // System generates Code, estruturas de output que chequea solver
  
  EvoltutionClass* Evolution = new EvoltutionClass();
  error = Evolution->start();checkError();
  
  SolverClass* Solver = new SolverClass();
  error = Solver->initialize();checkError(); // inicilaiza residuals, comunicadores, etc.
  
  while((*Evolution.status != END)){    
    error = Solver->iterateUntilConvergence();checkError();
    error = Evolution->update();checkError();
  }
  
  Solver->finish();
  
  newt.fini()//MPI
  
}
