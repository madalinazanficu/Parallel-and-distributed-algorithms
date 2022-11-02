#include <iostream>
#include <pthread.h>
#include "tema1.h"


/* Read line by line from input_file
   The first line reprsents the number of files 
   which will be distributed to mapper threads */
void parse_input(std::ifstream &read, std::queue<std::string> &q) {
    if (read.is_open()) {
        std::string line;
        getline(read, line);
        while(getline(read, line)) {
            q.push(line);
        }
    }
}

/* Compute all perfect powers and save them in a cache */
void precompute(std::unordered_map<int, std::unordered_set<int>> &cache_powers,
                int reducers_threads) {

    for (int i = 2; i <= reducers_threads + 1; i++) {
        std::unordered_set<int> perfect_power;
        cache_powers[i] = perfect_power;
    }
    for (int i = 2; i < sqrt(INT32_MAX); i++) {
        long x = i;
        int p = i;
        for (int j = 2; j <= reducers_threads + 1; j++) {
            x = x * p;
            if (x >= INT32_MAX) {
                break;
            }
            cache_powers[j].insert(x);
        }
    }
}

/* 
    Each mapper theard will read the associated input_file
    For each number from the file, the cache of powers is inspected

    The result is stored in a map where:
    -> the key - is a exponent/power
    -> the value - is a list conntaining perfect powers
 */
void mapper(std::string file, int thread_id, int exponents,
            std::unordered_map<int, std::unordered_set<int>> *cache_powers,
            std::unordered_map<int, std::vector<int>> &thread_result) {

    for (int i = 2; i <= exponents; i++) {
        std::vector<int> perfect_power;
        thread_result[i] = perfect_power;
    }

    std::ifstream read;
    std::string line;

    read.open(file);
    getline(read, line);
    while(getline(read, line)) {
        int x = stoi(line);

        for (int i = 2; i <= exponents; i++) {
            if (x == 1 || (*cache_powers)[i].find(x) != (*cache_powers)[i].end()) {
                thread_result[i].push_back(x);
            }
        }
    }
    read.close();
}

/* 
   Each reducer thread will take the correspondent exponent 
   from mappers'result.
    
   Mappers_result structure contains M results from mapper theards
   The result is described above 

   The output file for each reducer thread is: outX.txt
   where X = the correspondent exponent
*/
void reducer(int exponent, std::vector<std::unordered_map<int, 
                           std::vector<int>>> *mappers_result) {

    std::unordered_set<int> unique_numbers;
    for (unsigned long int i = 0; i < mappers_result->size(); i++) {
        for (int number : (mappers_result->at(i))[exponent]) {
            unique_numbers.insert(number);
        }
    }

    std::string output_file = "out" + std::to_string(exponent) + ".txt";
    std::ofstream write;

    write.open(output_file);
    write << unique_numbers.size();
    write.close();
}

void *thread_function(void *arg) {
    thread_arg_t* data = (thread_arg_t*) arg;
    std::queue<std::string> *q = data->q;
    int id = data->id;

    // Mapper theard action
    if (id < data->mapper_threads) {
        while (q->empty() == false) {
            // Distribute files to the curent mapper thread
            pthread_mutex_lock(data->mutex);
            if (q->empty() == true) {
                pthread_mutex_unlock(data->mutex);
                break;
            }
            std::string file = q->front();
            q->pop();
            pthread_mutex_unlock(data->mutex);

            // The result of the current thread
            std::unordered_map<int, std::vector<int>> thread_result;
            mapper(file, data->id, data->reducer_threads + 1, 
                    data->cache_powers, thread_result);

            // Lock this resource, only 1 thread can write once
            pthread_mutex_lock(data->mutex);
            data->mappers_result->push_back(thread_result);
            pthread_mutex_unlock(data->mutex);
        }
    }
    // Wait until all mapper threads finish the execution
    pthread_barrier_wait(data->barrier);

    // Reducer threads action
    if (id >= data->mapper_threads) {
        int assigned_exponent = id - data->mapper_threads + 2;
        reducer(assigned_exponent, data->mappers_result);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int mapper_threads = std::stoi(argv[1]);
    int reducer_threads = std::stoi(argv[2]);
    std::string input_file = argv[3];

    std::ifstream read;
    read.open(input_file);
    std::queue<std::string> q;
    std::unordered_map<int, std::unordered_set<int>> cache_powers;
    std::vector<std::unordered_map<int, std::vector<int>>> mappers_results;
    parse_input(read, q);
    read.close();

    // Precompute all powers
    precompute(cache_powers, reducer_threads);

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
        arg.q = &q;
        arg.mutex = &mutex;
        arg.reducer_threads = reducer_threads;
        arg.mapper_threads = mapper_threads;
        arg.mappers_result = &mappers_results;
        arg.cache_powers = &cache_powers;
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