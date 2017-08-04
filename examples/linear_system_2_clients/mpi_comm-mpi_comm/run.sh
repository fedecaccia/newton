hostname > mf
mpirun -np 1 -machinefile mf ${NEWTON_DIR}/bin/Newton : -np 1 ${NEWTON_DIR}/usr/bin/Test 0 0 mpi_comm 1 : -np 1 ${NEWTON_DIR}/usr/bin/Test 0 1 mpi_comm 2
