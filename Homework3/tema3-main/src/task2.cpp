#include "task2.h"


// ________________________________________________________________
// Leader to worder and worker to leader communication functions
// ________________________________________________________________

void send_to_worker(int start, int end, int dst, int *v, int N, int rank)
{
    print_message(rank, dst);
    MPI_Send(&start, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    MPI_Send(&end, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    MPI_Send(&N, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    for (int j = start; j <= end; j++) {
        MPI_Send(&v[j], 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
    }

}

int* receive_from_worker(int rank, int src, int *start, int *end, int N)
{
    MPI_Status status;
    MPI_Recv(start, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(end, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    int *recv_vec = (int *) calloc(N, sizeof(int));
    for (int i = *start; i <= *end; i++) {
        MPI_Recv(&recv_vec[i], 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
    }
    return recv_vec;
}

// ________________________________________________________________
// Leader to leader communication functions
// ________________________________________________________________

void send_to_next_leader(int N, int workload, int rank, 
                            int start, int end, int *v)
{
    int dst = (rank + 1) % leaders;
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
                            int rank, int *start, int *end) 
{
    MPI_Status status;
    int src = (rank - 1) % leaders;
    if (rank == 0) src = 3;
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

void send_to_my_leader(int my_leader, int start, int end, int *recv_vec, int rank)
{
    print_message(rank, my_leader);
    MPI_Send(&start, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD);
    MPI_Send(&end, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD);
    for (int i = start; i <= end; i++) {
        MPI_Send(&recv_vec[i], 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD);
    }
}


int *receive_from_leader(int *N, int my_leader, int *start, int *end)
{
    MPI_Status status;
    MPI_Recv(start, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(end, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(N, 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);

    int *recv_vec = (int *) calloc(*N, sizeof(int));
    for (int i = *start; i <= *end; i++) {
        MPI_Recv(&recv_vec[i], 1, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);
    }
    return recv_vec;
}


// ________________________________________________________________
// Communication logic 
// ________________________________________________________________

void distribute_work(int rank, int P, int **topology, int *N, 
                    vector<int> &cluster, int my_leader)
{
    if (rank == P0) {
        int *v = (int *) calloc(*N, sizeof(int));
        for (int k = 0; k < *N; k++) {
            v[k] = *N - k - 1;
        }
        int workers = count_workers(P, topology);

        // Workload for each worker
        int workload = ceil((double)*N / workers);

        // Send indexes and a partition of the vector to each worker
        int start = 0;
        int end = start + workload - 1;
        for (unsigned long int i = 0; i < cluster.size(); i++) {
            send_to_worker(start, end, cluster[i], v, *N, rank);
            start = end + 1;
            end = start + workload - 1;
        }

        // Send the workload data to the next leader
        send_to_next_leader(*N, workload, rank, start, end, v);

        // Ring structure is complete: P3 just finished
        int *recv_v = recv_from_prev_leader(N, &workload, rank, &start, &end);

    } else if (rank == P1 || rank == P2 || rank == P3) {
        int workload;
        int start, end;
        int *v = recv_from_prev_leader(N, &workload, rank, &start, &end);

        // Distribute the work to cluster's workers
        end = start + workload - 1;
        for (unsigned long int i = 0; i < cluster.size(); i++) {
            send_to_worker(start, end, cluster[i], v, *N, rank);
            start = end + 1;
            end = start + workload - 1;

            // The end index of P3 might be out of bounds, due to ceil approximation
            if (end > *N) {
                end = *N - 1;
            }
        }
        // Send the workload data to the next leader
        send_to_next_leader(*N, workload, rank, start, end, v);
    } 
}


void execute_computation(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader)
{
    // Workers execute the computation and send the result to the leader
    if (is_worker(rank) == true) {
        int start, end;
        int *recv_vec = receive_from_leader(N, my_leader, &start, &end);

        // Each worker will do the computation
        for (int i = start; i <= end; i++) {
            recv_vec[i] = recv_vec[i] * 5;
        }

        // Each worker will send the result to his leader
        send_to_my_leader(my_leader, start, end, recv_vec, rank);
    }

    // Leader receives the results from the workers and merges them
    int *v = (int *) calloc(*N, sizeof(int));
    int start, end;
    if (is_leader(rank) == true) {
        for (int i = 0; i < cluster.size(); i++) {
            int *recv_v = receive_from_worker(rank, cluster[i], &start, &end, *N);

            // Merge the results
            for (int j = start; j <= end; j++) {
                v[j] = recv_v[j];
            }
        }
    }

    // Sends the results on the ring structure
    MPI_Status status;
    if (rank == P0) {
        int dst = (rank + 1) % leaders;
        print_message(rank, dst);
        MPI_Send(v, *N, MPI_INT, dst, 0, MPI_COMM_WORLD);

        int src = (rank - 1) % leaders;
        if (rank == 0) src = 3;
        int *v_recv = (int *) calloc(*N, sizeof(int));
        MPI_Recv(v_recv, *N, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

        // Final result
        cout << "Rezultat: ";
        for (int i = 0; i < *N; i++) {
            cout << v_recv[i] << " ";
        }

    } else if (rank == P1 || rank == P2 || rank == P3) {
        int src = (rank - 1) % leaders;
        if (rank == 0) src = 3;
        int *v_recv = (int *) calloc(*N, sizeof(int));
        MPI_Recv(v_recv, *N, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

        // Merge the results
        for (int j = 0; j < *N; j++) {
            if (v[j] == 0) {
                v[j] = v_recv[j];
            }
        }

        int dst = (rank + 1) % leaders;
        print_message(rank, dst);
        MPI_Send(v, *N, MPI_INT, dst, 0, MPI_COMM_WORLD);
    }
}