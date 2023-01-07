#include "helper.h"


//____________________________________________________________
// TOPOLOGY FUNCTION (USED FOR TASK1 && TASK3)
// Stage 1: Leaders collaborate based on task structure
// Stage 2: The leaders send their topology to the workers
// Stage 3: The workers receive the topology from the leaders
//____________________________________________________________

int **get_topology_generic(int rank, int P, int my_leader, 
                    std::vector<int> cluster,
                    void (*leaders_collab_round0)(int, int**, int, int**),
                    void (*leaders_collab_round1)(int, int**, int, int**))
{
    int **topology = (int **) malloc(sizeof(int*) * 4);
    int **recv_topology = (int **) malloc(sizeof(int*) * 4);

    for (int i = 0; i < leaders; i++) {
		topology[i] = (int *) calloc(sizeof(int), P);
        recv_topology[i] = (int *) calloc(sizeof(int), P);
    }

    for (long unsigned int i = 0; i < cluster.size(); i++) {
        topology[rank][i] = cluster[i];
    }

    /* Stage 1: Leaders collaborate based on task structure */
    leaders_collab_round0(rank, topology, P, recv_topology);
    leaders_collab_round1(rank, topology, P, recv_topology);

    if (is_leader(rank) == true) {
        print_topology(rank, P, topology);
    }

    /* Stage 2: Send to workers */
    if (is_leader(rank) == true) {
        for (long unsigned int j = 0; j < cluster.size(); j++) {
            print_message(rank, cluster[j]);
            for (int i = 0; i < leaders; i++) {
                MPI_Send(topology[i], P, MPI_INT, 
                        cluster[j], 0, MPI_COMM_WORLD);
            }
        }
    }

    /* Stage 3: Receive the topology from leader and update his topo */
    if (is_worker(rank) == true) {
        MPI_Status status;
        for (int i = 0; i < leaders; i++) {
            MPI_Recv(topology[i], P, MPI_INT, my_leader, 
                        0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
        print_topology(rank, P, topology);
    }
    return topology;
}



// ________________________________________________________________
// Leader to worker and worker to leader communication functions
// ________________________________________________________________

void send_to_worker(int start, int end, int dst, 
                    int *v, int N, int rank)
{
    print_message(rank, dst);
    MPI_Send(&start, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    MPI_Send(&end, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    MPI_Send(&N, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    for (int j = start; j <= end; j++) {
        MPI_Send(&v[j], 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    }

}

int* receive_from_worker(int rank, int src, 
                        int *start, int *end, int N)
{
    MPI_Status status;
    MPI_Recv(start, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(end, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    int *v_recv = (int *) calloc(N, sizeof(int));
    for (int i = *start; i <= *end; i++) {
        MPI_Recv(&v_recv[i], 1, MPI_INT, 
                    src, 0, MPI_COMM_WORLD, &status);
    }
    return v_recv;
}


// ________________________________________________________________
// Leader to leader communication functions
// ________________________________________________________________

void send_to_next_leader(int N, int workload, int rank, 
                            int start, int end, int *v, int dst)
{
    print_message(rank, dst);
    MPI_Send(&workload, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    MPI_Send(&start, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    MPI_Send(&N, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);

    // Send the entire vector
    for (int i = 0; i < N; i++) {
        MPI_Send(&v[i], 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    }
}

int *recv_from_prev_leader(int *N, int *workload, 
                            int rank, int *start, int *end, int src) 
{
    MPI_Status status;
    MPI_Recv(workload, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(start, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(N, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

    // Receive the entire vector from the previous leader
    int *v = (int *) calloc(*N, sizeof(int));
    for(int i = 0; i < *N; i++) {
        MPI_Recv(&v[i], 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    }
    return v;
}



// ________________________________________________________________
// Worker to leader and leader to worker communication functions
// ________________________________________________________________

void send_to_my_leader(int my_leader, int start, 
                        int end, int *v_recv, int rank)
{
    print_message(rank, my_leader);
    MPI_Send(&start, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD);
    MPI_Send(&end, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD);
    for (int i = start; i <= end; i++) {
        MPI_Send(&v_recv[i], 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD);
    }
}


int *receive_from_leader(int *N, int my_leader, int *start, int *end)
{
    MPI_Status status;
    MPI_Recv(start, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(end, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(N, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);

    int *v_recv = (int *) calloc(*N, sizeof(int));
    for (int i = *start; i <= *end; i++) {
        MPI_Recv(&v_recv[i], 1, MPI_INT, my_leader, 
                    0, MPI_COMM_WORLD, &status);
    }
    return v_recv;
}



// ________________________________________________________________
// Helper functions
// ________________________________________________________________

void print_topology(int rank, int P, int **topology) {
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

void print_message(int src, int dst) {
    std::cout << "M(" << src << "," << dst << ")" << endl;
}

bool is_leader(int rank) {
    if (rank == P0 || rank == P1 || rank == P2 || rank == P3) {
        return true;
    }
    return false;
}

bool is_worker(int rank) {
    if (rank != P0 && rank != P1 && rank != P2 && rank != P3) {
        return true;
    }
    return false;
}

int minn(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int maxx(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int count_workers(int P, int **topology)
{
    int workers = 0;
    for (int i = 0; i < leaders; i++) {
        for (int j = 0; j < P; j++) {
            if (topology[i][j] != 0) {
                workers++;
            }
        }
    }
    return workers;
}