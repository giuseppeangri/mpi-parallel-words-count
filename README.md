

# MPI Parallel Words Count

Academic Project submitted by [**Giuseppe Angri**](https://github.com/giuseppeangri)

Master Degree Course: **Parallel and Concurrent Programming and on the Cloud**

Teacher: **Vittorio Scarano**

[Computer Science Department](http://www.di.unisa.it) - [University of Salerno](https://www.unisa.it/) (Italy)

## Problem statement

We will be doing a version of map-reduce using MPI to perform word counting over a large number of files. There are 3 steps to this process:
1. is to read in the master file list which will contain the names of all the files that are to be counted. Note that only 1 of your processes should read this file. Then each of the processes should receive their portion of the file from the master process. Once a process has received its list of files to process, it should then read in each of the files and perform a word counting, keeping track of the frequency each word found in the files occurs. We will call the histogram produced the local histogram. This is similar to the map stage or map-reduce.
2. is to then combine frequencies of words across processes. For example the word 'cat' might be counted in multiple processes and we need to add up all these occurrences. This is similar to the reduce stage of map-reduce. 
3. is to have each of the processes send their local histograms to the master process. The master process just needs to gather up all this information. Note that there will be duplicate words between processes. The master should then print out the results to the screen.

## Benchmarking

1. Provide a solution to the problem exploiting parallel computation and develop  a C program using MPI. The provided implementation must use Collective communication routines.
2. Benchmark the solution on Amazon AWS EC2 instances (m4.xlarge). Testing the solution using 1, 2, 3, 4, 5, 6, 7, 8 instances.
3. Both weak and strong scalability have to be analyzed:
	- Strong Scaling: Keeping the problem size fixed and pushing in more workers or processors. Goal: Minimize time to solution for a given problem.
	- Weak Scaling: Keeping the work per worker fixed and adding more workers/processors (the overall problem size increases). Goal: solve the larger problems.

## Input files

This project has been realized to read and perform a words count on Plain Text Files (instead of files having just one word for each line).

Example:

> Lorem ipsum integer hac dui ac mollis vel, sollicitudin nullam metus
> aenean hendrerit conubia mattis, sagittis elementum aliquet enim
> dictumst eros ut phasellus maecenas congue erat volutpat phasellus non
> turpis dictum dolor maecenas et accumsan, class sapien netus arcu
> condimentum est velit quisque dapibus pharetra varius tortor, tempor
> justo commodo aenean nisi ad erat litora arcu enim lacinia bibendum
> pharetra vitae proin fusce aliquam sapien dapibus consequat tristique
> scelerisque rutrum odio ornare, in fames sapien fermentum eu justo
> feugiat leo consectetur amet.

## Algorithm description

The algorithm has been implemented to reach the goal of having a perfect division of the problem between the workers.

To reach this goal, instead of split the problem by the number of files or by the numer of rows, the approach is to read the size in byte of each file, make the sum, and split by the number of workers. In this way each workers read and perform the words count on the same size of problem.

This approach splits the problem in the best way but makes it more diffucult to manage. 

Each files can have different size and each worker must process a fixed size. So, can happen that a worker must work on a single file, on multiple files, on a file already started by previous worker.

To handle this situations, at first each worker must calculate the part processed by previous workers and then start to process its part.

To manage the read and the words count, has been implemented a file reader that handle all conditions: when it starts from beginning of a file, inside a word, on a space, on a new line, on an EOF, etc.

When a worker end its part inside a word, this is ignored and is processed by the next worker.

When a worker start its part inside a word, it come back until the space to process the word ignored by the previous worker.

All the words readed are added in the apposite structure (described below) that representes the Local Histogram.

At the end, each workers make the MPI pack of its structure and send it to the master, that merge them to make the Global Histogram.

## Implementation details

### MPI features

In this project has been used the following MPI features:

 - MPI_Bcast
 - MPI_Gather
 - MPI_Gatherv
 - MPI_Pack
 - MPI_Unpack

### Custom Structures

Some custom structures have been defined:

 - **File Information**
    
    Contains the information of a file to read: 
    - the path (of dynamic length)
    - the size of a file

 - **File Information Container**
    
    Is a dynamic list of **File Information** structure, contains:
    - the entries
    - the number of entries
    - the total size of the files
 
	There are the main functions to manage the entries and an additional library to manage the communication of **File Information Container** structure through	 MPI.

 - **Counter**
 
    Keeps track of the frequency of a single word, contains:
	  - the word  (of dynamic length)
	  - the counter

 - **Counter Container**
 
    Is a dynamic list of **Counter** structure, contains:
	  - the entries
	  - the number of entries
 
	There are the main functions to manage the entries and an additional library to manage the communication of **Counter Container** structure through MPI and to make the merge of multiple **Counter Container** structures coming to the master process from the workers.

### Logs

It has been implemented an MPI library for the print of messages and logs in the standard output and in the log files.

## Tests and Results

The tests were performed on Amazon AWS EC2 cluster, with 8 instances (m4.xlarge).

Each instance has 4 vCPUs, so the tests were performed by increasing the number of processors by one at a time, and by adding other instaces gradually (starting from 1 instance with 1 processor, until 8 instances with 32 processors).

The single input file used for the tests contains: 
 - 10000 words
 - 67.159 bytes

### Weak Scalability

For the tests have been used 5 files for each processor, by increasing the size of problem gradually.

The resulting graph shows a linear execution time.

There is only a small difference in the first runs (with a single instance), but then the trend is very stable.

![Weak Scalability](tests-results/weak-scalability.png)

### Strong Scalability

For the tests have always been used 160 files, for a total of:
- 160000 words
- 10745440 bytes

In this way the size of the problem is fixed, and gradually other processors have been added to the cluster.

In fact the graph shows that the execution time has decreased up to **13 times**.

![Strong Scalability](tests-results/strong-scalability.png)

## Instructions

### Compile

Inside the ***src*** directory run the following command:

    make

### Local Execution

Inside the ***src*** directory run the following command:

    mpirun -np NUMBER_OF_PROCESSORS main LIST_OF_FILES

 - **NUMBER_OF_PROCESSORS**
 
   Is the number of processors that MPI will use for the execution.
	
 - **LIST_OF_FILES** (optional)
   
   Is the path of the file that contains the paths of all files to be read and counted.
   
   If omitted, it will be used the file: ***"data/00_master.txt"*** 

### Cluster Execution	

Inside the ***src*** directory run the following command:

    mpirun -np NUMBER_OF_PROCESSORS --host MASTER_IP,WORKER_01_IP,WORKER_02_IP main LIST_OF_FILES

 - **NUMBER_OF_PROCESSORS**
   
   Is the number of processors that MPI will use for the execution.

- **MASTER_IP / WORKER_01_IP / WORKER_02_IP / ...** 
  
  Are the IP addresses of the instances to use. These must be divided by a commas without spaces.
	
 - **LIST_OF_FILES** (optional)
   
   Is the path of the file that contains the paths of all files to be read and counted.

   If omitted, it will be used the file: ***"data/00_master.txt"*** 


