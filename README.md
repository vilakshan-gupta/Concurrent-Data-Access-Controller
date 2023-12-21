# Concurrent-Data-Access-Controller

# Description
This project demonstrates a robust solution for managing concurrent data access in a multi-threaded environment using C. It features a detailed implementation of reader-writer synchronization, where writer threads have exclusive access to a shared 1024-byte character array, and reader threads have limited simultaneous access, employing advanced synchronization techniques like timed wait and try wait with exponential backoff.

# Installation
# Prerequisites
GCC Compiler (or any C compiler)
POSIX compliant operating system for pthreads and semaphores

# Steps to Install
1. Clone the repository:
   bash
``` git clone [URL of the repository]```
2. Navigate to the project directory:
   bash
```cd Concurrent-Data-Access-Controller```
