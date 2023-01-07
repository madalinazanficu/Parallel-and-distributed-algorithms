#ifndef __TASK3__
#define __TASK3__

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "mpi.h"
#include "helper.h"

using namespace std;

// int **get_topology(int rank, int P, int my_leader,
//                     std::vector<int> cluster, int task);

int **get_topology_task3(int rank, int P, int my_leader, 
                        std::vector<int> cluster);

void leader_colllab_round1(int rank, int **topology, 
                            int P, int **recv_topology);

void leaders_collab_round0(int rank, int **topology, 
                            int P, int **recv_topology);

int *recv_from_prev_leader(int *N, int *workload, 
                        int rank, int *start, int *end, int src);

void send_to_next_leader(int N, int workload, int rank, 
                            int start, int end, int *v, int dst);

void distribute(int rank, int P, int **topology, int *N, 
                    vector<int> &cluster, int my_leader);

void computation(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);

#endif  // __TASK3__