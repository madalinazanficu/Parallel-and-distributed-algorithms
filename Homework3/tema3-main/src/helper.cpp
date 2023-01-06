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