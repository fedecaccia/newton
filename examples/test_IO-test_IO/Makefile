MAIN_DIR=../../.
EXAMPLE_DIR=.
DEP_DIR=$(MAIN_DIR)/inc
SRC_DIR=$(EXAMPLE_DIR)/src
OBJ_DIR=$(EXAMPLE_DIR)/obj
BIN_DIR=$(EXAMPLE_DIR)/bin
OBJELEM = $(OBJ_DIR)/main.o 						\
					$(OBJ_DIR)/MathLib.o
EXE=$(BIN_DIR)/Test
MPICC=mpic++

all: $(BIN_DIR) $(OBJ_DIR) $(OBJELEM)
	$(MPICC) $(OBJELEM) -o $(EXE)

#~ 	$(FC) $(OBJELEM) \
#~ 	$(FFLAGS) -o $(EXE) \
#~ 	$(PETSC_FORTRAN_LIB) $(PETSC_LIB) ${SLEPC_SYS_LIB} -lz

clean_:
	$(RM) $(OBJELEM) $(EXE)

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	$(MPICC) -c -o $@ $< -I${DEP_DIR}
#~ 	${PETSC_COMPILE} -c ${CFLAGS} -o $@ $< -I${DEP_DIR}

#~ include ${PETSC_DIR}/lib/petsc/conf/variables
#~ include ${SLEPC_DIR}/lib/slepc/conf/slepc_common
