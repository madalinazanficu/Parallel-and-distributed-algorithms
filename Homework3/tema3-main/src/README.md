### **Zanficu Madalina - Homework3 - MPI**

### **Files**:
The main function is in tema3.cpp.
Each task has its indivitual .cpp and .h file:
- task1.cpp / task1.h
- task2.cpp / task2.h
- task3.cpp / task3.h
- task4.cpp / task4.h

The helper.cpp file contains methods used by all tasks.

### **Implementation**
- **Task1 - Topology**
For this task, I have implemented a generic method found in helper.cpp:
In order to make it generic I passed pointer to functions (params 5, 6, 7).
**get_topology_generic(int rank, 
                        int P, 
                        int my_leader, 
                        std::vector<int> cluster,
                        void (*leaders_collab_round0)(int, int**, int, int**),
                        void (*leaders_collab_round1)(int, int**, int, int**),
                        void (*print_topology_f)(int, int, int **, int))
                    
My main idea was:
- **Stage 1**: Each leader will complete his own row in topology matrix.
- **Stage 2**: Each leader will collaborate with other leaders 
to share their topology. So for this part, each tasks implements 
his own method of collaboration. For task1: **the collaboration is on a ring.**
So, both first and second round of collaboration is **P0->P1->P2->P3->P0.**
After two rounds, each leader will know the entire topology.

- **Stage 3**: Each leader will send the completed topology to their workers.
- **Stage 4**: Workers receive the topology.

**Task2 - Collaborate computation between processes**
**The first step is distributing the computational tasks to workers (distribute_task2)**.

- **Stage1** : P0 is computing the workload based on the number 
of workers and the array size. So each worker process 
have to work between 2 indices: start and end.

- **Stage2**: P0 will send start/end indices to his workers. 
P0 sends the workload, the start index, and the array to the next leader.

- **Stage 3**: Each leader will send start/end indices to
their workers and will continue to communicate with their 
next leader is order to complete the ring collaboration.

**The second part involves doing the computation and collecting the results (computation_task2)**
- Only worker processes can do the calculation.
They are responsible for receiving the start/end pointers 
from their leaders, multiplying their part of the array 
by 5 and sending the result back to the leader.

- Leaders will collect the results from their workers 
and collaborate between each other on the ring structure 
(P0->P1->P2->P3->P0) in order to get the final result.

**Task3**
- **The difference** between task3 and task1/task2 **is the collaboration between leaders**.

- For topology, I call the generic topology method using 
pointers to leaders_collab_round0 and leaders_collab_round1.
- leaders_collab_round0 spreads the topology between leaders,
based on this linear communication (P0-> P3 -> P2 -> P1),
and leader_collab_round1 assures that P0 will know the
final topology (P1 -> P2 -> P3 -> P0).

- For the computation: distribute_task3 and computation_task3 
are mostly the same as the ones from task2, but as previously 
described the communication between leaders is linear.
**P0-> P3 -> P2 -> P1 - for distributing the workload**
**P1 -> P2 -> P3 -> P0 - for collecting the final results back in P0**

**Task4**
- The communication between P1 and other leader is blocked
Leaders' collaboration respects this: 
**P0 -> P3 -> P2 && P2 -> P3 -> P0**

- Another difference was computing the workload, excluding 
P1 workers.

### **Generic methods of communication used** - helper.cpp:
1. **Leader ro worker and worker to leader**
send_to_worker / receive_from_worker

2. **Leader to leader**
send_to_next_leader / recv_from_prev_leader

3. **Worker to leader and leader to worker**
send_to_my_leader / receive_from_leader
















