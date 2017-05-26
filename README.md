# MPI Parallel Words Count

Academic Project for the course: Parallel and Concurrent Programming on the Cloud.

Professor: Vittorio Scarano

Submitted by: Giuseppe Angri

### Problem statement

We will be doing a version of map-reduce using MPI to perform word counting over a large number of files. There are 3 steps to this process:
1) is to read in the master file list which will contain the names of all the files that are to be counted. Note that only 1 of your processes should read this file. Then each of the processes should receive their portion of the file from the master process. Once a process has received its list of files to process, it should then read in each of the files and perform a word counting, keeping track of the frequency each word found in the files occurs. We will call the histogram produced the local histogram. This is similar to the map stage or map-reduce.
2) is to then combine frequencies of words across processes. For example the word 'cat' might be counted in multiple processes and we need to add up all these occurrences. This is similar to the reduce stage of map-reduce. 
3) is to have each of the processes send their local histograms to the master process. The master process just needs to gather up all this information. Note that there will be duplicate words between processes. The master should then print out the results to the screen.

### Benchmarking

1) Provide a solution to the problem exploiting parallel computation and develop  a C program using MPI. The provided implementation must use Collective communication routines.
2) Benchmark the solution on Amazon AWS (EC2) on Compute optimize instances (e.g. C3.large family). Testing the solution using 1, 2, 3, 4, 5, 6, 7, 8 instances.
3) Both weak and strong scalability have to be analyzed:
- Strong Scaling: Keeping the problem size fixed and pushing in more workers or processors. Goal: Minimize time to solution for a given problem.
- Weak Scaling: Keeping the work per worker fixed and adding more workers/processors (the overall problem size increases). Goal: solve the larger problems.

###### HINT

1) The results should be presented as two different scatter x-y charts, where the x-axis denotes the number of MPI processors used and the y-axis value represents the time in milliseconds.  
2) The number of MPI processors should be equal to the number of cores.
