#ifndef __TASK4__
#define __TASK4__

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "mpi.h"
#include "helper.h"

using namespace std;

int **get_topology_task4(int rank, int P, int my_leader, 
                        std::vector<int> cluster);

void leaders_collab_r0(int rank, int **topology,
                        int P, int **recv_topology);

void leaders_collab_r1(int rank, int **topology,
                        int P, int **recv_topology);

void print_topology_task4(int rank, int P, int **topology, 
                            int my_leader);

void distribute_task4(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);

void computation_task4(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);

#endif  // __TASK4__