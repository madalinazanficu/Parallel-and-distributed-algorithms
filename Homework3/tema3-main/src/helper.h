#ifndef __HELPER__
#define __HELPER__

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

using namespace std;

// Clusters' leaders
#define P0 0
#define P1 1
#define P2 2
#define P3 3
#define leaders 4

void print_message(int src, int dst);
bool is_leader(int rank);
bool is_worker(int rank);
int maxx(int a, int b);
int minn(int a, int b);
int count_workers(int P, int **topology);

#endif  // __HELPER__
