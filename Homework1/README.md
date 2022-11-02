Zanficu Madalina-Valentina 333CA

Parallel and Distributed Algorithms - Homework1

### **Main idea**
**Parallel program** for searching perfect powers, bigger than 0,
in a set of files and counting the unique values for each exponent.
The paralelization is executed based on the **Map-Reduce model**, 
with 2 types of threads: mapper and reducer.

The input files are **dynamically distributed** (at runtime) to 
mapper threads, resulting into partial lists for each exponent. 
**Reducers combines** all lists and **agregate them** for each exponent.
**Each reducer thread will count in parallel** the number of unique
values associed with the assigned exponent.

### **Notations:**
reducer_threads = the number of reducer threads
mapper_threads = the number of mapper threads 
exponents = the number of exponents = reducer_threads + 1

Formula for the assigned exponent to each reducer thread:
**assigned_exponent = thread_id - mapper_threads + 2;**
Because the first exponent is 2 and mapper threads have smaller ids.

mapper_thread   id     |        reducer_thread   id    assigned_exponent
1                0     |             1            4            2
2                1     |             2            5            3
3                2     |             3            6            4
4                3     |

### **Data-structures**
Tema1.h

thread_arg_t contains:
-> **barrier && mutex**: synchronization elements
-> **a queue of input files**, which will be processed by mapper threads
-> **cache_powers:** a cache for all perfect powers, for speeding up the 
computational process (Firsly, I try to check for each number if is a 
perfect power using decomposition into prime factors with O(sqrt(N)) 
time complexity, but I got TLE)

-> **mappers_result**: a list which contains **results from all mapper threads**
(a result from a mapper consists in a structure which mantains numbers found
in the associated input file for each exponent)


### **Implementation**
1. Precompute: all perfect powers for exponents: 
    2, 3, .... (reducers_threads + 1)

2. Initialization for barrier and mutex
Important: the barrier is init with the total number of threads
Following the logic from thread_function, the reducers will go
directly to the barrier and more important we need to wait until
all mapper threads finish the execution.

3. Create all threads in the same time and execute theard_function
4. **Thread_function:**

Structure:
**---> Mapper threads area**

In parallel: each thread will read the associated input file
and search in cache for perfect powers found in the file.
(also calls **maper function**)

**Possible race-conditions and problems**:
a) threads try to extract the same file from the queue
**Solution:** lock that area with a mutex

b) one of the thread was blocked by the mutex, 
but meantime the queue has been emptied 
**Solution:** double check the state of the queue before
extracting an element

c) deadlock: the queue remains empty, but the thread is locked
**Solution:** unlock the thread in empty condition

d) Multiple threads try to push back their resource.
**Solution:** lock that resource

barrier --------------------------------------------------

**---> Reducer theards area**
As explained before, an assigned_exponent for each thread is computed.
Each theard will traverse all results from mapper theards and filter
all list associated with the exponent.
At the end it will open a file and write the number of unique values found.
(also calls **reducer function**)