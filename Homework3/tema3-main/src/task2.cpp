#include "task2.h"


// ________________________________________________________________
// Communication logic 
// ________________________________________________________________

void distribute_task2(int rank, int P, int **topology, int *N, 
                    vector<int> &cluster, int my_leader)
{

    int dst = (rank + 1) % leaders;
    int src = (rank - 1) % leaders;
    if (rank == 0) src = 3;

    if (rank == P0) {
        int *v = (int *) calloc(*N, sizeof(int));
        for (int k = 0; k < *N; k++) {
            v[k] = *N - k - 1;
        }
        int workers = count_workers(P, topology);
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
        int dst = (rank + 1) % leaders;
        send_to_next_leader(*N, workload, rank, start, end, v, dst);

        // Ring structure is complete: P3 just finished
        int *v_recv = recv_from_prev_leader(N, &workload, rank, &start, &end, src);

    } else if (rank == P1 || rank == P2 || rank == P3) {
        int workload;
        int start, end;
        int *v = recv_from_prev_leader(N, &workload, rank, &start, &end, src);

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
        send_to_next_leader(*N, workload, rank, start, end, v, dst);
    } 
}


void computation_task2(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader)
{
    // Workers execute the computation and send the result to the leader
    if (is_worker(rank) == true) {
        int start, end;
        int *v_recv = receive_from_leader(N, my_leader, &start, &end);

        // Each worker will do the computation
        for (int i = start; i <= end; i++) {
            v_recv[i] = v_recv[i] * 5;
        }

        // Each worker will send the result to his leader
        send_to_my_leader(my_leader, start, end, v_recv, rank);
    }

    // Leader receives the results from the workers and merges them
    int *v = (int *) calloc(*N, sizeof(int));
    int start, end;
    if (is_leader(rank) == true) {
        for (int i = 0; i < cluster.size(); i++) {
            int *v_recv = receive_from_worker(rank, cluster[i], &start, &end, *N);
            merge_results(v, v_recv, start, end);
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
        merge_results(v, v_recv, 0, *N - 1);

        int dst = (rank + 1) % leaders;
        print_message(rank, dst);
        MPI_Send(v, *N, MPI_INT, dst, 0, MPI_COMM_WORLD);
    }
}