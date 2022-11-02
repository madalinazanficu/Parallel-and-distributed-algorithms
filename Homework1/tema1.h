#ifndef TEMA1_H_
#define TEMA1_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>
#include <fstream>
#include <algorithm>

typedef struct thread_arg_t thread_arg_t;
struct thread_arg_t
{
	int id;                                 // the id of the current thread
	int mapper_threads;				        // number of mappers theards
	int reducer_threads;			    	// number of reducer threads 
	pthread_mutex_t* mutex;			   	    // syncronization element
	pthread_barrier_t* barrier;		  		// syncronization element			
	std::queue<std::string>* q;		        // input files to be processed


	// The cached perfect powers
	std::unordered_map<int, std::unordered_set<int>> *cache_powers;

	// The overall result from all mapper theards
	std::vector<std::unordered_map<int, std::vector<int>>> *mappers_result;
};

/*
	The first input file contain a list of N input files
	These files are stored in a queue until mapper theards
	analyze them.
*/
void parse_input(std::ifstream &read, std::queue<std::string> &q);

/* Caching all perfect powers */
void precompute(std::unordered_map<int, std::unordered_set<int>> &cache_powers,
                int reducers_threads);


/* Mapper theards functionality */
void mapper(std::string file, int thread_id, int exponents,
            std::unordered_map<int, std::unordered_set<int>> *cache_powers,
            std::unordered_map<int, std::vector<int>> &thread_result);

/* Reducer therads functionality */
void reducer(int exponent, std::vector<std::unordered_map<int, 
                           std::vector<int>>> *mappers_result);

/* 
	Functionality for both mapper and reducers threads
	All threads are created in the same time, so reducer
	theads need to wait untill all mapers finish their execution
*/
void *thread_function(void *arg);

#endif  // TEMA1_H_