hostname > mf

ompi-server -r ompiuri
sleep 1
HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
${NEWTON_DIR}/bin/Newton < /dev/null >& 0-newton.log &
sleep 1
HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
${NEWTON_DIR}/usr/bin/Test 1 0 mpi_port 0 < /dev/null >& 1-test0.log &
sleep 1
HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
${NEWTON_DIR}/usr/bin/Test 1 1 mpi_port 1 < /dev/null >& 2-test1.log &
sleep 1
HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
${NEWTON_DIR}/usr/bin/Test 1 2 mpi_port 2 < /dev/null >& 3-test2.log &
sleep 1
killall ompi-server
