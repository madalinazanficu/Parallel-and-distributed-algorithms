#ifndef __TASK3__
#define __TASK3__

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "mpi.h"
#include "helper.h"

using namespace std;

int **get_topology_task3(int rank, int P, int my_leader, 
                        std::vector<int> cluster);

void leader_colllab_round1(int rank, int **topology, 
                            int P, int **recv_topology);

void leaders_collab_round0(int rank, int **topology, 
                            int P, int **recv_topology);

void distribute_task3(int rank, int P, int **topology, int *N, 
                    vector<int> &cluster, int my_leader);

void computation_task3(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);

#endif  // __TASK3__