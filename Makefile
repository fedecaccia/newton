MAIN_DIR=.
DEP_DIR=$(MAIN_DIR)/inc
SRC_DIR=$(MAIN_DIR)/src
OBJ_DIR=$(MAIN_DIR)/obj
BIN_DIR=$(MAIN_DIR)/bin
OBJELEM = $(OBJ_DIR)/Communicator.o	\
		  $(OBJ_DIR)/Evolution.o	\
		  $(OBJ_DIR)/global.o		\
		  $(OBJ_DIR)/main.o 		\
		  $(OBJ_DIR)/Mapper.o 		\
		  $(OBJ_DIR)/Newton.o 		\
		  $(OBJ_DIR)/Parser.o 		\
		  $(OBJ_DIR)/System.o 		\
		  $(OBJ_DIR)/Solver.o
EXE=$(BIN_DIR)/Newton
MPICC=mpic++

all: $(BIN_DIR) $(OBJ_DIR) $(OBJELEM)
	$(MPICC) $(OBJELEM) -o $(EXE)

#~ 	$(FC) $(OBJELEM) \
#~ 	$(FFLAGS) -o $(EXE) \
#~ 	$(PETSC_FORTRAN_LIB) $(PETSC_LIB) ${SLEPC_SYS_LIB} -lz

clean_:
	$(RM) $(OBJELEM) $(OBJELEM2) $(EXE) $(EXETEST)

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	$(MPICC) -c -o $@ $< -I${DEP_DIR}
#~ 	${PETSC_COMPILE} -c ${CFLAGS} -o $@ $< -I${DEP_DIR}

#~ include ${PETSC_DIR}/lib/petsc/conf/variables
#~ include ${SLEPC_DIR}/lib/slepc/conf/slepc_common
