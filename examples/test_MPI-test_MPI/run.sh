ompi-server -r ompiuri

HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
../../bin/Newton < /dev/null >& 0-newton.log &

HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
bin/Test 0 dummy mpi < /dev/null >& 1-test0.log &

HOSTFILE=mf
SLURM_NPROCS=1
mpiexec --prefix ${MPI_DIR} \
-ompi-server file:ompiuri \
-machinefile ${HOSTFILE} -n ${SLURM_NPROCS} \
bin/Test 1 dummy mpi < /dev/null >& 2-test1.log &

killall ompi-server
