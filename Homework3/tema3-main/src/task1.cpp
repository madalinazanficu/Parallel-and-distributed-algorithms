#include "task1.h"


int **get_topology_task1(int rank, int P, 
                        std::vector<int> &cluster, int my_leader)
{
    int **topology = get_topology_generic(rank, P, my_leader, cluster, 
                                            ring_collab, ring_collab, print_topology);
    return topology;
}


void ring_collab(int rank, int **topology, int P, int **recv_topology) {

    MPI_Status status;
    int dst = (rank + 1) % leaders;
    int src = (rank - 1) % leaders;

    // P0 starts the ring
    if (rank == P0) {
        
        // Send line by line his topology
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }

        // Receive the topology from the last leader
        for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

            // Update his topology with the received one
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
    } else if (rank == P1 || rank == P2 || rank == P3) {

        // Receive the topology from the previous leader
        for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

            // Update his topology with the received one
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }

        // Send line by line his topology
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }
    }
}