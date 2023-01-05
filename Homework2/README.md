###Zanficu Madalina 333CA

## Homework2 - APD

## Main idea
The program aims to distribute shopping orders to multiple
workers. Each order contains Q products, and each product
is found in different storage areas. In order to boost the 
performance of shipping multiple orders, there are 2 parallelization levels.
- level 1 is responsible to allocate orders to P workers
- for level 2, each worker (which is responsible for an order) 
  distribute tasks of searching products to other P workers.

## Files Structure:
Tema2.java     
MyThread.java - used for reading orders file (orders.txt)
MyTask.java   - used for reading products file (order_products.txt)

## Workflow and Implementation
The homework is divided into 3 stages:
1. *Main stage in Tema2.java*
The main goal is to distribute orders equally to P threads.
In order to divide orders.txt as equal as possible, I computed
the total size of the file in bytes and divided it to P threads.
So each thread has a chunk of bytes between startIndex and 
endIndex to read from orders.txt.

The main problem is when the start index of a thread is not 
located at the start of a line. So start indexes 
will be adjusted in stage2, while the file is being read.

2. *Second stage in MyThread.java*
Each thread will execute .run() method, which firstly
reads the bytes from startIndex and endIndex. In case
startIndex is not at the beginning of the line, I move the start index
on the next line. Same situtation, in case the endIndex is not at the
end of a line, I move it at the end of the previous line.

Once each thread knows exactly which orders it has to process,
It reads the orders between startIndex and endIndex and I will 
land tasks for searching products in order_products.txt.
These tasks are submitted to a thread pool and for each order
I submit exactly 1 task.

*The mechanism of waiting until all products from an order are shipped*
In order to complete an order, I need to wait for all products (Q) to be shipped.
For this, I used a semaphore (each order has its own semaphore).
The semaphore is initialized with -Q + 1.
Before submitting a task, the main thread will acquire the semaphore,
and everytime a product from the current order is found, it will release
the semaphore. After Q products, the semaphore will not be aquired anymore, 
so the current order is completed.

3. *Third stage in MyTask.java*
This class represents the action of a task from thread pool.
Each task has assigned an orderId and a fileIndex where it 
starts reading from order_products.txt.
Each task will read line by line starting from fileIndex until 
the thread finds a product from the assigned order.

The thread will land a new task, to find another product 
and change the fileIndex to the current position (in bytes), 
so the next thread will start reading from the current position.
Also, when a product from a specific order is found, the current
task will release the semafore for the assigned order.


## Writing in output files
I have implemented 2 methods:
- ProductShipped (MyTask.java) for writing the output in order_products.out
- OrderCompleted (MyThread.java) for writing the output in orders.out
I used syncronized (lock on writerOrders and writerProducts) because
I wanted only one thread to write in a file at a moment of time.