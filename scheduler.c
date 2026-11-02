#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#define NUM_PROCS   3
#define QUANTUM_SEC 2
#define WORK_STEPS  8

pid_t pids[NUM_PROCS];
int done[NUM_PROCS];
int current = -1;
int alive = NUM_PROCS;
volatile sig_atomic_t tick = 0;

// timer interrupt: only raise a flag, the real work happens in main
void on_tick(int sig) {
    (void)sig;
    tick = 1;
}

// the "program" each child runs
void child_work(int id) {
    raise(SIGSTOP);  // wait here until the scheduler resumes us
    for (int step = 1; step <= WORK_STEPS; step++) {
        printf("  [%c] step %d/%d\n", 'A' + id, step, WORK_STEPS);
        fflush(stdout);
        usleep(500000);
    }
    exit(0);
}

// round robin: next process that is not finished
int next_alive(int from) {
    for (int k = 1; k <= NUM_PROCS; k++) {
        int idx = (from + k) % NUM_PROCS;
        if (!done[idx]) return idx;
    }
    return -1;
}

void schedule(void) {
    if (current >= 0 && !done[current]) {
        if (waitpid(pids[current], NULL, WNOHANG) == pids[current]) {
            done[current] = 1;
            alive--;
            printf("[scheduler] %c finished\n", 'A' + current);
        } else {
            kill(pids[current], SIGSTOP);  // quantum expired: preempt
        }
    }

    int nxt = next_alive(current);
    if (nxt < 0) return;  // everything finished

    current = nxt;
    printf("[scheduler] switch to %c\n", 'A' + current);
    kill(pids[current], SIGCONT);
}

int main(void) {
    // create children; each one stops itself right away
    for (int i = 0; i < NUM_PROCS; i++) {
        pids[i] = fork();
        if (pids[i] == 0) child_work(i);
        waitpid(pids[i], NULL, WUNTRACED);  // wait until the child is stopped
    }

    struct sigaction sa = {0};
    sa.sa_handler = on_tick;
    sigaction(SIGALRM, &sa, NULL);

    // block SIGALRM so a tick cannot slip in between the check and the wait
    sigset_t block, old;
    sigemptyset(&block);
    sigaddset(&block, SIGALRM);
    sigprocmask(SIG_BLOCK, &block, &old);

    struct itimerval timer = {{QUANTUM_SEC, 0}, {QUANTUM_SEC, 0}};
    setitimer(ITIMER_REAL, &timer, NULL);

    schedule();  // start the first process

    while (alive > 0) {
        while (!tick) sigsuspend(&old);  // sleep until the timer fires
        tick = 0;
        schedule();
    }

    struct itimerval off = {{0, 0}, {0, 0}};
    setitimer(ITIMER_REAL, &off, NULL);
    printf("[scheduler] all processes finished\n");
    return 0;
}
