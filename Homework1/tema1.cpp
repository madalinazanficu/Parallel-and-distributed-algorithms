#include <iostream>
#include <string>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "utils.h"
#include "tema1.h"

using namespace std;

/* Read line by line from input_file
   First line -> reprsents the number of files which will be distributed to threads */
void parse_input(std::ifstream &read, std::vector<std::string> &files) {
    if (read.is_open()) {
        std::string line;
        getline(read, line);
        while(getline(read, line)) {
            files.push_back(line);
        }
    }
}

/* Check if the number x is a perfect power of e */
bool check_perfect_number(int n, int e) {
    std::unordered_map<int, int> prime_fact;

    int p = 2;
    while(n % p == 0) {
        prime_fact[p]++;
        n = n / p;
    }

    for (p = 3; p * p <= n; p = p + 2) {
        while(n % p == 0) {
            prime_fact[p]++;
            n = n / p;
        }
    }

    if (n > 2) {
        prime_fact[n]++;
    }

    /* For each key in the map, his value should be divisible with e */
    for (auto it = prime_fact.begin(); it != prime_fact.end(); it++) {
        if (it -> second % e != 0) {
            return false;
        }
    }
    return true;
}

//TODO: Mapper function for thread execution


//TODO: Reduce function for thread execution


int main(int argc, char *argv[]) {
    
    DIE(argc < 4, "Not enough arguments!");

    int mapper_threads = stoi(argv[1]);
    int reducer_threads = stoi(argv[2]);
    std::string input_file = argv[3];

    std::ifstream read;
    read.open(input_file);
    std::vector<std::string> files;
    parse_input(read, files);
    read.close();

    //TODO: Create threads
    // int threads_number = mapper_threads + reducer_threads;
    // pthread_t threads[threads_number];
    // std::vector<thread_arg_t> threads_arg;

    // for (int i = 0; i < threads_number; i++) {
    //     thread_arg_t arg;
    //     arg.thread_id = i;
    //     threads_arg.push_back(arg);

    //     pthread_create(&threads[i], NULL, thread_function, &threads_arg[i]);
    // }

    //TODO: Distribute files to mapper threads

    //TODO: Wait until all mapper threads finish the execution (apply barrier) 

    //TODO: Distribute list of exponents power to reducer threads

    // Delete threads
    // for (int i = 0; i < threads_number; i++) {
	// 	pthread_join(threads[i], NULL);
	// } 

    return 0;
}