#include "task3.h"

/*
* P0-> P3 -> P2 -> P1
* Each leader sends his topology to the next one
* and receives the topology from the previous one
* and updates his topology with the received one
*/
void leaders_collab_round0(int rank, int **topology, int P, int **recv_topology) {
    MPI_Status status;
    if (rank == P0) {
        int dst = P3;
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }
    } else if (rank == P1 || rank == P2 || rank == P3) {
        int src = rank + 1;
        if (rank == P3) {
            src = P0;
        }
         for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
        if (rank != P1) {
            int dst = rank - 1;
            print_message(rank, dst);
            for (int i = 0; i < 4; i++) {
                MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
            }
        }
    }
}

/*
* P1 -> P2 -> P3 -> P0
* Each leader sends his topology to the next one
* and receives the topology from the previous one
* and updates his topology with the received one
*/
void leader_colllab_round1(int rank, int **topology, int P, int **recv_topology) {
    MPI_Status status;
    if (rank == P1) {
        int dst = rank + 1;
        print_message(rank, dst);
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }
    } else if (rank == P0 || rank == P2 || rank == P3) {
        int src = rank - 1;
        if (rank == P0) {
            src = P3;
        }
        for (int i = 0; i < 4; i++) {
            MPI_Recv(recv_topology[i], P, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < P; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = recv_topology[i][j];
                }
            }
        }
        if (rank == P2 || rank == P3) {
            int dst = rank + 1;
            if (rank == P3) {
                dst = P0;
            }
            print_message(rank, dst);
            for (int i = 0; i < 4; i++) {
                MPI_Send(topology[i], P, MPI_INT, dst, 0, MPI_COMM_WORLD);
            }
        }
    } 
}

int **get_topology_task3(int rank, int P, int my_leader, 
                        std::vector<int> cluster)
{

    int **topology = get_topology_generic(rank, P, my_leader, cluster, 
                        leaders_collab_round0, leader_colllab_round1);
    return topology;
}

// int **get_topology(int rank, int P, int my_leader, std::vector<int> cluster, int task)
// {
//     int **topology = (int **) malloc(sizeof(int*) * 4);
//     int **recv_topology = (int **) malloc(sizeof(int*) * 4);

//     for (int i = 0; i < leaders; i++) {
// 		topology[i] = (int *) calloc(sizeof(int), P);
//         recv_topology[i] = (int *) calloc(sizeof(int), P);
//     }

//     for (long unsigned int i = 0; i < cluster.size(); i++) {
//         topology[rank][i] = cluster[i];
//     }

//     leaders_collab_round0(rank, topology, P, recv_topology);
//     leader_colllab_round1(rank, topology, P, recv_topology);

//     if (is_leader(rank) == true) {
//         print_topology(rank, P, topology);
//     }

//     /* Stage 2: Send to workers */
//     if (is_leader(rank) == true) {
//         for (long unsigned int j = 0; j < cluster.size(); j++) {
//             print_message(rank, cluster[j]);
//             for (int i = 0; i < leaders; i++) {
//                 MPI_Send(topology[i], P, MPI_INT, cluster[j], 0, MPI_COMM_WORLD);
//             }
//         }
//     }

//     if (is_worker(rank) == true) {
//         MPI_Status status;
//         for (int i = 0; i < leaders; i++) {
//             MPI_Recv(topology[i], P, MPI_INT, my_leader, 0, MPI_COMM_WORLD, &status);
//             // Update his topology with the received one
//             for (int j = 0; j < P; j++) {
//                 if (topology[i][j] == 0) {
//                     topology[i][j] = recv_topology[i][j];
//                 }
//             }
//         }
//         print_topology(rank, P, topology);
//     }
//     return topology;
// }


void distribute(int rank, int P, int **topology, int *N, 
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

    } else if (rank == P1 || rank == P2 || rank == P3) {
        int workload;
        int start, end;
        int src = rank + 1;
        if (rank == P3) {
            src = P0;
        }
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
        if (rank != P1) {
            int dst = rank - 1;
            send_to_next_leader(*N, workload, rank, start, end, v, dst);
        }
    }
}

void computation(int rank, int P, int **topology, int *N, 
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
        for (long unsigned int i = 0; i < cluster.size(); i++) {
            int *recv_v = receive_from_worker(rank, cluster[i], &start, &end, *N);

            // Merge the results
            for (int j = start; j <= end; j++) {
                v[j] = recv_v[j];
            }
        }
    }

    // Send the results back to P0
    MPI_Status status;
    if (rank == P1) {
        int dst = rank + 1;
        print_message(rank, dst);
        MPI_Send(v, *N, MPI_INT, dst, 0, MPI_COMM_WORLD);
    } else if (rank == P2 || rank == P3) {
        int src = rank - 1;
        int *v_recv = (int *) calloc(*N, sizeof(int));
        MPI_Recv(v_recv, *N, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

        // Merge the results
        for (int j = 0; j < *N; j++) {
            if (v[j] == 0) {
                v[j] = v_recv[j];
            }
        }
        int dst = rank + 1;
        if (rank == P3) {
            dst = P0;
        }
        print_message(rank, dst);
        MPI_Send(v, *N, MPI_INT, dst, 0, MPI_COMM_WORLD);
    } else if (rank == P0) {
        int src = P3;
        int *v_recv = (int *) calloc(*N, sizeof(int));
        MPI_Recv(v_recv, *N, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

        // Merge the results
        for (int j = 0; j < *N; j++) {
            if (v[j] == 0) {
                v[j] = v_recv[j];
            }
        }
        // Final result
        cout << "Rezultat: ";
        for (int i = 0; i < *N; i++) {
            cout << v[i] << " ";
        }
        cout << endl;
    }
}

