MAIN_DIR=.
DEP_DIR=$(MAIN_DIR)/inc
SRC_DIR=$(MAIN_DIR)/src
USR_SRC_DIR=$(MAIN_DIR)/usr/src
USR_DEP_DIR=$(MAIN_DIR)/usr/inc
OBJ_DIR=$(MAIN_DIR)/obj
USR_OBJ_DIR=$(MAIN_DIR)/usr/obj
BIN_DIR=$(MAIN_DIR)/bin
USR_BIN_DIR=$(MAIN_DIR)/usr/bin
OBJELEM = $(OBJ_DIR)/Client.o						   				\
					$(OBJ_DIR)/Communicator.o	       				\
					$(OBJ_DIR)/Debugger.o	           				\
					$(OBJ_DIR)/Evolution.o	         				\
					$(OBJ_DIR)/global.o		  								\
					$(OBJ_DIR)/main.o 		  								\
					$(OBJ_DIR)/Mapper.o 										\
					$(OBJ_DIR)/MathLib.o 										\
					$(OBJ_DIR)/Newton.o 										\
					$(OBJ_DIR)/Parser.o 										\
					$(OBJ_DIR)/System.o 										\
					$(OBJ_DIR)/Solver.o							 				\
					$(OBJ_DIR)/testClient.o									\
					$(OBJ_DIR)/testMap.o
USR_OBJELEM = 	$(USR_OBJ_DIR)/loadXSfromCRM.o	  \
								$(USR_OBJ_DIR)/neutronicCr2kP.o 	\
								$(USR_OBJ_DIR)/neutronicKP2cr.o 	\
								$(USR_OBJ_DIR)/pow2spow.o					\
								$(USR_OBJ_DIR)/spow2fpow.o				\
								$(USR_OBJ_DIR)/spowk2fpowk.o			\
								$(USR_OBJ_DIR)/th2xs.o						\
								$(USR_OBJ_DIR)/relapPow2th.o			\
								$(USR_OBJ_DIR)/fermiXs2pow.o			\
								$(USR_OBJ_DIR)/userClient.o       \
								$(USR_OBJ_DIR)/userMap.o
TEST_OBJELEM = $(USR_OBJ_DIR)/mainTest.o		
EXE=$(BIN_DIR)/Newton
TEST_EXE=$(USR_BIN_DIR)/Test
MPICC=mpic++ -std=c++11 

all: newton test
	
newton: $(BIN_DIR)  $(OBJ_DIR) $(OBJELEM) $(USR_OBJ_DIR) $(USR_OBJELEM)
	$(MPICC) $(OBJELEM) $(USR_OBJELEM) \
	$(FFLAGS) -o $(EXE) \
	$(PETSC_FORTRAN_LIB) $(PETSC_LIB) ${SLEPC_SYS_LIB} -lz

test: $(BIN_DIR) $(OBJ_DIR) $(TEST_OBJELEM)
	$(MPICC) $(TEST_OBJELEM) -o $(TEST_EXE)

clean_:
	$(RM) $(OBJELEM) $(USR_OBJELEM) $(TEST_OBJELEM) $(EXE) $(TEST_EXE)

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	${PETSC_COMPILE} -c ${CFLAGS} -o $@ $< -I${DEP_DIR} -I${USR_DEP_DIR}
	
${USR_OBJ_DIR}/%.o: ${USR_SRC_DIR}/%.cpp
	${PETSC_COMPILE} -c ${CFLAGS} -o $@ $< -I${DEP_DIR} -I${USR_DEP_DIR}

include ${PETSC_DIR}/lib/petsc/conf/variables
include $(PETSC_DIR)/lib/petsc/conf/rules
