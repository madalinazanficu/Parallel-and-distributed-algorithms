#ifndef __TASK2__
#define __TASK2__

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "mpi.h"
#include "helper.h"

using namespace std;

void distribute_task2(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);

void computation_task2(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);
#endif  // __TASK2__