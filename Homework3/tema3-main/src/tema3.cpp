#include "tema3.h"

using namespace std;

// Clusters' leaders
#define P0 0
#define P1 1
#define P2 2
#define P3 3
#define leaders 4


void read_cluster(int rank, std::vector<int> &cluster) {
    string filename = "cluster" + std::to_string(rank) + ".txt";
    ifstream file(filename);

    int i = 0;
    int nodes;
    string line;
    
    while (getline(file, line)) {
        if (i == 0) {
            nodes = stoi(line);
            i++;
        } else {
            cluster.push_back(stoi(line));
        }
    }
    file.close();
}

/*
* Each cluster leader will inform his workers that he is the leader
* Each worker will be informed by his leader
*/
void inform_workers(int rank, int &my_leader, std::vector<int> &cluster) {
    if (is_leader(rank) == true) {
        for (long unsigned int i = 0; i < cluster.size(); i++) {
            MPI_Send(&rank, 1, MPI_INT, cluster[i], 0, MPI_COMM_WORLD);
        }
    }
    if (is_worker(rank) == true) {
        MPI_Status status;
        MPI_Recv(&my_leader, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    }
}


int main(int argc, char * argv[]) {
    int P;
    int rank;
    int N = 0;
    int comm_err;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    comm_err = atoi(argv[2]);

    // Only P0 knows the N
    if (rank == P0) {
        N = atoi(argv[1]);
    }

    // Cluster
    std::vector<int> cluster;

    // Read clusters
    if (is_leader(rank) == true) {
        read_cluster(rank, cluster);
    }

    // Inform each worker about his leader
    int my_leader = -1;
    inform_workers(rank, my_leader, cluster);


    if (comm_err == 0) {
        int **topology = task1(rank, P, cluster, my_leader);
        distribute_work(rank, P, topology, &N, cluster, my_leader);
        execute_computation(rank, P, topology, &N, cluster, my_leader);
    } else if (comm_err == 1) {
        int **topology = get_topology_task3(rank, P, my_leader, cluster);
        distribute(rank, P, topology, &N, cluster, my_leader);
        computation(rank, P, topology, &N, cluster, my_leader);
    }

    MPI_Finalize();
	return 0;
}