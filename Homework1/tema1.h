#ifndef TEMA1_H_
#define TEMA1_H_

#include <string>
#include <vector>
#include <unordered_map>

typedef struct thread_arg_t thread_arg_t;
struct thread_arg_t
{
	int id;
	int reducer_threads;
	bool type; 												// mapper or reducer thread
	pthread_mutex_t* mutex;
	void* files;								
	void* mapped_files;
};


#endif  // TEMA1_H_