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

int **get_topology_generic(int rank, int P, int my_leader, 
                    std::vector<int> cluster,
                    void (*leaders_collab_r0)(int, int**, int, int**),
                    void (*leaders_collab_r1)(int, int**, int, int**),
                    void (*print_topology_f)(int, int, int **, int));


void send_to_worker(int start, int end, int dst, int *v, int N, int rank);
int* receive_from_worker(int rank, int src, int *start, int *end, int N);


void send_to_next_leader(int N, int workload, int rank, 
                            int start, int end, int *v, int dst);

int *recv_from_prev_leader(int *N, int *workload, 
                            int rank, int *start, int *end, int src);

void send_to_my_leader(int my_leader, int start, int end, int *recv_vec, int rank);
int *receive_from_leader(int *N, int my_leader, int *start, int *end);


void print_message(int src, int dst);
void print_topology(int rank, int P, int **topology, int my_parent);
bool is_leader(int rank);
bool is_worker(int rank);
int maxx(int a, int b);
int minn(int a, int b);
int count_workers(int P, int **topology);
void merge_results(int *v, int *v_recv, int start, int end);

#endif  // __HELPER__
