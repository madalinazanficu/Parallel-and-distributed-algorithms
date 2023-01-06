#include "helper.h"

void print_message(int src, int dst) {
    std::cout << "M(" << src << "," << dst << ")" << endl;
}

bool is_leader(int rank) {
    if (rank == P0 || rank == P1 || rank == P2 || rank == P3) {
        return true;
    }
    return false;
}

bool is_worker(int rank) {
    if (rank != P0 && rank != P1 && rank != P2 && rank != P3) {
        return true;
    }
    return false;
}

int minn(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int maxx(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int count_workers(int P, int **topology)
{
    // Find the number of workers
    int workers = 0;
    for (int i = 0; i < leaders; i++) {
        for (int j = 0; j < P; j++) {
            if (topology[i][j] != 0) {
                workers++;
            }
        }
    }
    return workers;
}