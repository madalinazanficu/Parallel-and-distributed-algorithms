#include <iostream>
#include <string>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>

using namespace std;

typedef struct thread_arg_t thread_arg_t;
struct thread_arg_t
{
    int id;
    int reducer_threads;
    int mapper_threads;
    pthread_mutex_t* mutex;
    pthread_barrier_t* barrier;
    void* files;
    std::queue<std::string>* q;
    std::unordered_map<int, std::vector<int>> *perfect_powers;
    std::vector<std::unordered_map<int, std::vector<int>>> *mappers_result;
};


/* Read line by line from input_file
   First line -> reprsents the number of files which will be distributed to threads */
void parse_input(std::ifstream &read, std::vector<std::string> &files,
                    std::queue<std::string> &q) {

    if (read.is_open()) {
        std::string line;
        getline(read, line);
        while(getline(read, line)) {
            files.push_back(line);
            q.push(line);
        }
    }
}

// Compute all perfect powers
void precompute(int reducers, std::unordered_map<int, std::vector<int>> &perfect_powers) {
    for (int i = 2; i <= reducers + 1; i++) {
        std::vector<int> perfect_power;
        perfect_powers[i] = perfect_power;
    }

    for (int i = 2; i < sqrt(INT32_MAX); i++) {
        long x = i;
        int p = i;
        for (int j = 2; j <= reducers + 1; j++) {
            x = x * p;
            if (x >= INT32_MAX) {
                break;
            }
            perfect_powers[j].push_back(x);
        }
    }
}



/* Mapper function for thread execution */
void mapper(std::string file, int thread_id, int exponents,
            std::unordered_map<int, std::vector<int>> *perfect_powers,
            std::unordered_map<int, std::vector<int>> &thread_result) {

    // Init the map of lists of perfect powers
    for (int i = 2; i <= exponents; i++) {
        std::vector<int> perfect_power;
        thread_result[i] = perfect_power;
    }

    // Read line by line from file
    std::ifstream read;
    std::string line;

    read.open(file);
    getline(read, line);
    while(getline(read, line)) {
        int x = stoi(line);

        for (int i = 2; i <= exponents; i++) {
            if (x == 1 || binary_search((*perfect_powers)[i].begin(), (*perfect_powers)[i].end(), x)) {
                thread_result[i].push_back(x);
            }
        }
    }
    read.close();
}

// Each reducer thread will take the correspondent exponent
void reducer(int exponent, vector<unordered_map<int, vector<int>>> *mappers_result) {
    unordered_set<int> unique_numbers;
    for (unsigned long int i = 0; i < mappers_result->size(); i++) {
        // unordered_map<int, vector<int>> mapper_result = (*mappers_result)[i];

        // Search the exponent
        // auto iterator = mappers_result->at(i).find(exponent);
        // vector<int> perfect_numbers = iterator->second;

        for (int number : (mappers_result->at(i))[exponent]) {
            unique_numbers.insert(number);
        }
    }

    std::string output_file = "out" + to_string(exponent) + ".txt";
    std::ofstream write;

    write.open(output_file);
    write << unique_numbers.size();
    write.close();
}

void *thread_function(void *arg) {
    thread_arg_t* data = (thread_arg_t*) arg;
    std::queue<std::string> *q = data->q;
    std::vector<std::unordered_map<int, std::vector<int>>> *mappers_result = data->mappers_result;
    int id = data->id;

    // Distribute files to mapper threads
    if (id < data->mapper_threads) {
        while (q->empty() == false) {
            pthread_mutex_lock(data->mutex);
            if (q->empty() == true) {
                pthread_mutex_unlock(data->mutex);
                break;
            }
            std::string file = q->front();
            q->pop();
            pthread_mutex_unlock(data->mutex);

            std::unordered_map<int, std::vector<int>> thread_result;
            mapper(file, data->id, data->reducer_threads + 1, data->perfect_powers, thread_result);
            pthread_mutex_lock(data->mutex);
            mappers_result->push_back(thread_result);
            pthread_mutex_unlock(data->mutex);
        }
    }
    // Wait until all mapper threads finish the execution
    pthread_barrier_wait(data->barrier);


    if (id >= data->mapper_threads) {
        // Distribute list of exponents power to reducer threads
        int exponent = id - data->mapper_threads + 2;
        vector<unordered_map<int, vector<int>>> *mappers_result = data->mappers_result;
        reducer(exponent, mappers_result);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int mapper_threads = stoi(argv[1]);
    int reducer_threads = stoi(argv[2]);
    std::string input_file = argv[3];

    std::ifstream read;
    read.open(input_file);
    std::vector<std::string> files;
    std::queue<std::string> q;
    std::vector<std::unordered_map<int, std::vector<int>>> mappers_results;
    std::unordered_map<int, std::vector<int>> perfect_powers;
    parse_input(read, files, q);
    read.close();

    // Precompute all powers
    precompute(reducer_threads, perfect_powers);

    // Create threads
    int threads_number = mapper_threads + reducer_threads;
    pthread_t threads[threads_number];
    thread_arg_t threads_arg[threads_number];

    // Init mutex
    pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

    // Init barrier
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, threads_number);

    for (int i = 0; i < threads_number; i++) {
        thread_arg_t arg;
        arg.id = i;
        arg.files = &files;
        arg.q = &q;
        arg.mutex = &mutex;
        arg.reducer_threads = reducer_threads;
        arg.mapper_threads = mapper_threads;
        arg.mappers_result = &mappers_results;
        arg.perfect_powers = &perfect_powers;
        arg.barrier = &barrier;
        threads_arg[i] = arg;
        
        pthread_create(&threads[i], NULL, thread_function, &threads_arg[i]);
    }

    // Delete threads
    for (int i = 0; i < threads_number; i++) {
		pthread_join(threads[i], NULL);
	}

    // Destroy mutex
	pthread_mutex_destroy(&mutex); 

    return 0;
}