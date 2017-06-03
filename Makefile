# Macros
MAIN_DIR=.
DEP_DIR=$(MAIN_DIR)/inc
SRC_DIR=$(MAIN_DIR)/src
OBJ_DIR=$(MAIN_DIR)/obj
BIN_DIR=$(MAIN_DIR)/bin
OBJELEM = $(OBJ_DIR)/Solver.o 		\
		  $(OBJ_DIR)/main.o
EXE=$(BIN_DIR)/newton
MPICC=mpic++

# Reglas explicitas
all: $(BIN_DIR) $(OBJ_DIR) $(OBJELEM)
	$(MPICC) $(OBJELEM) -o $(EXE)

#~ 	$(FC) $(OBJELEM) \
#~ 	$(FFLAGS) -o $(EXE) \
#~ 	$(PETSC_FORTRAN_LIB) $(PETSC_LIB) ${SLEPC_SYS_LIB} -lz

clean_:
	$(RM) $(OBJELEM) $(OBJELEMTEST) $(EXE) $(EXETEST)

# Creacion de objetos
${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	$(MPICC) -c -o $@ $< -I${DEP_DIR}
#~ 	${PETSC_COMPILE} -c ${CFLAGS} -o $@ $< -I${DEP_DIR}

#~ include ${PETSC_DIR}/lib/petsc/conf/variables
#~ include ${SLEPC_DIR}/lib/slepc/conf/slepc_common
