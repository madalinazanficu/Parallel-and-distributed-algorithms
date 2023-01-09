#include "task4.h"

int **get_topology_task4(int rank, int P, int my_leader, 
                        std::vector<int> cluster) {
    int **topology = get_topology_generic(rank, P, my_leader, cluster, 
                                        leaders_collab_r0, leaders_collab_r1,
                                        print_topology_task4);
    return topology;
}

/*
* Leaders collaboration: P0-> P3 -> P2
*/
void leaders_collab_r0(int rank, int **topology,
                        int P, int **recv_topology) {
    MPI_Status status;
    if (rank == P0) {
        int dst = P3;
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }

    } else if (rank == P3) {
        int src = P0;
        for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
        int dst = P2;
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }

    } else if (rank == P2) {
        int src = P3;
        for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
    }
}

/*
* Leaders collaboration: P2 -> P3 -> P0
*/
void leaders_collab_r1(int rank, int **topology,
                        int P, int **recv_topology) {
    MPI_Status status;
    if (rank == P2) {
        int dst = P3;
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }
    } else if (rank == P3) {
        int src = P2;
        for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }

        int dst = P0;
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }

    } else if (rank == P0) {
        int src = P3;
        for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
    }
}

/*
* P0 -> P3 -> P2
*/
void distribute_task4(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader) {

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
        int dst = P3;
        send_to_next_leader(*N, workload, rank, start, end, v, dst);
    } else if (rank == P3 || rank == P2) {

        int workload;
        int start, end;
        int src;
        if (rank == P3) {
            src = P0;
        } else {
            src = P3;
        }
        int *v = recv_from_prev_leader(N, &workload, rank, &start, &end, src);

        // Distribute the work to cluster's workers
        end = start + workload - 1;
        for (unsigned long int i = 0; i < cluster.size(); i++) {
            send_to_worker(start, end, cluster[i], v, *N, rank);
            start = end + 1;
            end = start + workload - 1;
            if (end > *N) {
                end = *N - 1;
            }
        }
        if (rank == P3) {
            int dst = P2;
            send_to_next_leader(*N, workload, rank, start, end, v, dst);
        }
    }                      
}


void computation_task4(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader)
{
    // Workers execute the computation and send the result to the leader
    if (is_worker(rank) == true && my_leader != P1) {
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
    if (is_leader(rank) == true && rank != P1) {
        for (long unsigned int i = 0; i < cluster.size(); i++) {
            int *v_recv = receive_from_worker(rank, cluster[i], &start, &end, *N);
            merge_results(v, v_recv, start, end);
        }
    }

    // Send the result back to P0
    MPI_Status status;
    if (rank == P2) {
        int dst = P3;
        print_message(rank, dst);
        MPI_Send(v, *N, MPI_INT, dst, 0, MPI_COMM_WORLD);

    } else if (rank == P3 || rank == P0) {
        int src = rank - 1;
        if (rank == P0) {
            src = P3;
        }
        
        int *v_recv = (int *) calloc(*N, sizeof(int));
        MPI_Recv(v_recv, *N, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
        merge_results(v, v_recv, 0, *N - 1);

        if (rank == P3) {
            int dst = P0;
            print_message(rank, dst);
            MPI_Send(v, *N, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }

        if (rank == P0) {
            // Final result
            cout << "Rezultat: ";
            for (int i = 0; i < *N; i++) {
                cout << v[i] << " ";
            }
            cout << endl;
        }
    }
}


void print_topology_task4(int rank, int P, int **topology, int my_leader) {
    cout << rank << " -> ";

    if (rank != P1 && my_leader != P1) {
        for (int i = 0; i < leaders; i++) {
            if (i == P1) {
                continue;
            }
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
    } else {
        int i = P1;
        int j = 0;
        cout << i << ":";
        while (j + 1 < P && topology[i][j + 1] != 0) {
            cout << topology[i][j] << ",";
            j++;
        }
        if (topology[i][j] != 0) {
            cout << topology[i][j] << " ";
        }
        cout << endl;
    }
}