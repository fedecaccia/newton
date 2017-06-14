ompi-server -r ompiuri
sleep 1
HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
../../bin/Newton < /dev/null >& 0-newton.log &
sleep 1
HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
bin/Test 0 < /dev/null >& 1-test0.log &
sleep 1
HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
bin/Test 1 < /dev/null >& 2-test1.log &
sleep 1
killall ompi-server
