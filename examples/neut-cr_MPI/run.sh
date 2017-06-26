ompi-server -r ompiuri
sleep 1
HOSTFILE=mf
SLURM_NPROCS=2
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
../../bin/cr mpi_port 0 0 < /dev/null >& 1-cr.log &
sleep 1
killall ompi-server
