#ifndef __TASK1__
#define __TASK1__

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "mpi.h"
#include "helper.h"

using namespace std;


void print_task1(int rank, int P, int **topology);
void ring_collab(int rank, int **topology, int P, int **recv_topology);
int** task1(int rank, int P, std::vector<int> &cluster, int my_leader);

#endif  // __TASK1__