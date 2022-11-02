#ifndef TEMA1_H_
#define TEMA1_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

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
	std::vector<std::unordered_map<int, std::vector<int>>> *mappers_result;
	std::unordered_map<int, std::vector<int>> *perfect_powers;
};


#endif  // TEMA1_H_