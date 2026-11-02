# Time-Sharing Simulation (Round Robin)

Operating Systems AOL Case Study - COMP6697001

## Authors
- Iyurichie Lay
- Jeremy Emmanuel Susilo
- Yonami Trisandora

## Environment
- OS: Kali GNU/Linux Rolling 2026.2 (VMware)
- Compiler: GCC 15.2.0
- Build script: build.sh (bash)

## Build and run
    ./build.sh

Or manually:

    gcc -Wall scheduler.c -o scheduler
    ./scheduler

## How it works
- The parent process creates 3 child processes (A, B, C). Each child stops itself with SIGSTOP at startup.
- A timer (setitimer) sends SIGALRM every quantum (2 seconds).
- The signal handler only raises a flag. The main loop acts as the scheduler.
- On every tick, the running process is preempted with SIGSTOP and the next process is resumed with SIGCONT (Round Robin).
- Finished processes are reaped with waitpid and removed from the rotation.

## Files
- scheduler.c: main program
- tick.c: timer experiment
- fork_test.c: SIGSTOP/SIGCONT experiment
# os
