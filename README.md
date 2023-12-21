# Concurrent-Data-Access-Controller

# Description
This project demonstrates a robust solution for managing concurrent data access in a multi-threaded environment using C. It features a detailed implementation of reader-writer synchronization, where writer threads have exclusive access to a shared 1024-byte character array, and reader threads have limited simultaneous access, employing advanced synchronization techniques like timed wait and try wait with exponential backoff.

# Installation

# Prerequisites
GCC Compiler (or any C compiler)
POSIX compliant operating system for pthreads and semaphores

# Steps to Install

1. Clone the repository:
```
git clone [URL of the repository]
```
2. Navigate to the project directory:

```
cd Concurrent-Data-Access-Controller
```
# Usage 

To compile the program, run: 
```
gcc -pthread -o program src/*.c
```
To execute the program, use:

```
./program [M] [N] [R]
```
Where 'M' is the number of writer threads, 'N' is the number of reader threads, and R is the maximum number of readers that can access the shared data simultaneously.

# Features 

Multithreading: Utilizes POSIX threads for creating a multi-reader, single-writer scenario.

Synchronization: Employs semaphores for synchronization to ensure safe access to shared data.

Performance Optimization: Implements exponential backoff for readers to optimize performance and resource usage.



