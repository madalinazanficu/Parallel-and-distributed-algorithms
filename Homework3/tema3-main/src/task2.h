#ifndef __TASK2__
#define __TASK2__

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "mpi.h"
#include "helper.h"

using namespace std;

int count_workers(int P, int **topology);

void send_to_worker(int start, int end, int dst, int *v, int N, int rank);

// void send_to_next_leader(int N, int workload, int rank, 
//                             int start, int end, int *v);

// int *recv_from_prev_leader(int *N, int *workload, 
//                         int rank, int *start, int *end);

void distribute_work(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);

int *receive_from_leader(int my_leader, int *start, int *end);

void send_to_my_leader(int my_leader, int start, int end, int *recv_vec);

void execute_computation(int rank, int P, int **topology, int *N, 
                        vector<int> &cluster, int my_leader);

int* receive_from_worker(int rank, int src, int *start, int *end, int N);

#endif  // __TASK2__