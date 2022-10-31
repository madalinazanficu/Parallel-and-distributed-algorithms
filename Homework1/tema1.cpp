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
void parse_input(std::ifstream &read, std::vector<std::string> &files,
                    std::unordered_map<std::string, bool> &mapped_files) {

    if (read.is_open()) {
        std::string line;
        getline(read, line);
        while(getline(read, line)) {
            files.push_back(line);
            mapped_files[line] = false;
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

/* Mapper function for thread execution */
void mapper(std::string file, int thread_id, int exponents) {
    std::cout << "Thread-ul: " << thread_id << " : ";  
    std::cout << file << std::endl;


    // Init the map of lists of perfect powers
    std::unordered_map<int, std::vector<int>> perfect_numbers;
    for (int i = 2; i <= exponents; i++) {
        std::vector<int> perfect_power;
        perfect_numbers[i] = perfect_power;
    }

    // Read line by line from file
    std::ifstream read;
    std::string line;

    read.open(file);
    getline(read, line);
    while(getline(read, line)) {
        int x = stoi(line);

        for (int e = 2; e <= exponents; e++) {
            if (check_perfect_number(x, e) == true || x == 1) {
                perfect_numbers[e].push_back(x);
            }
        }
    }
    read.close();

    for (auto it = perfect_numbers.begin(); it != perfect_numbers.end(); it++) {
        std::cout << "Exponent: " << it->first << " ";
        for (int i = 0; i < it->second.size(); i++) {
            std::cout << it->second.at(i) << " ";
        }
        std::cout << endl; 
    }
}


//TODO: Reduce function for thread execution
void reducer(std::string file) {

}



void *thread_function(void *arg) {
    thread_arg_t* data = (thread_arg_t*) arg;
    std::vector<std::string> *files = (std::vector<std::string> *) data->files;
    std::unordered_map<std::string, bool> *mapped_files = (std::unordered_map<std::string, bool> *) data->mapped_files;

    for (std::string file : *files) {

        // Check if the file was mapped and TODO: lock this resource with a mutex
        if ((*mapped_files)[file] == false) {
            pthread_mutex_lock(data->mutex);
            mapper(file, data->id, data->reducer_threads + 1);
            (*mapped_files)[file] = true;
            pthread_mutex_unlock(data->mutex);

        }
    }

    // // TODO: add barrier
    // // TODO: Implement reducer

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    
    DIE(argc < 4, "Not enough arguments!");

    int mapper_threads = stoi(argv[1]);
    int reducer_threads = stoi(argv[2]);
    std::string input_file = argv[3];

    std::ifstream read;
    read.open(input_file);
    std::vector<std::string> files;
    std::unordered_map<std::string, bool> mapped_files;
    parse_input(read, files, mapped_files);
    read.close();

    // Init mutex
    pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

    //TODO: Create threads
    int threads_number = mapper_threads + reducer_threads;
    pthread_t threads[threads_number];
    thread_arg_t threads_arg[threads_number];

    for (int i = 0; i < threads_number; i++) {
        thread_arg_t arg;
        arg.id = i;
        if (i < reducer_threads) {
            arg.type = 0;
        } else {
            arg.type = 1;
        }
        arg.files = &files;
        arg.mapped_files = &mapped_files;
        arg.mutex = &mutex;
        arg.reducer_threads = reducer_threads;

        threads_arg[i] = arg;

        pthread_create(&threads[i], NULL, thread_function, &threads_arg[i]);
    }

    //TODO: Distribute files to mapper threads

    //TODO: Wait until all mapper threads finish the execution (apply barrier) 

    //TODO: Distribute list of exponents power to reducer threads

    // Delete threads
    for (int i = 0; i < threads_number; i++) {
		pthread_join(threads[i], NULL);
	}

    // Destroy mutex
	pthread_mutex_destroy(&mutex); 

    return 0;
}