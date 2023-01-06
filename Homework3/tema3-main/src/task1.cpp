#include "task1.h"


void print_task1(int rank, int P, int **topology) {
    cout << rank << " -> ";
    for (int i = 0; i < leaders; i++) {
        cout << i << ":";
        int j = 0;
        while (j + 1 < P && topology[i][j + 1] != 0) {
            cout << topology[i][j] << ",";
            j++;
        }
        if (topology[i][j] != 0) {
            cout << topology[i][j] << " ";
        }
    }
    cout << endl;
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

/*
* STAGE1: Leaders send their topology to the next leader 
* This is the ring collab structure P0 -> P1 -> P2 -> P3 -> P0.
*
* STAGE2: All leaders will send the final topology to the workers
*/
int** task1(int rank, int P, std::vector<int> &cluster, int my_leader) {

    int **recv_topology = (int **) malloc(sizeof(int*) * 4);
    int **topology = (int **) malloc(sizeof(int*) * 4);
    for (int i = 0; i < leaders; i++) {
		topology[i] = (int *) calloc(sizeof(int), P);
        recv_topology[i] = (int *) calloc(sizeof(int), P);
    }
    for (long unsigned int i = 0; i < cluster.size(); i++) {
        topology[rank][i] = cluster[i];
    }

    /* Stage 1 */
    ring_collab(rank, topology, P, recv_topology);
    ring_collab(rank, topology, P, recv_topology);
    if (is_leader(rank) == true) {
        print_task1(rank, P, topology);
    }

    /* Stage 2*/
    if (is_leader(rank) == true) {
        for (long unsigned int j = 0; j < cluster.size(); j++) {
            print_message(rank, cluster[j]);
            for (int i = 0; i < leaders; i++) {
                MPI_Send(topology[i], P, MPI_INT, cluster[j], 0, MPI_COMM_WORLD);
            }
        }
    }
    if (is_worker(rank) == true) {
        MPI_Status status;
        for (int i = 0; i < leaders; i++) {
            MPI_Recv(topology[i], P, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);

            // Update his topology with the received one
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
        print_task1(rank, P, topology);
    }

    return topology;
}