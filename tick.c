#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

void on_tick(int sig) {
    write(1, "tick\n", 5);
}

int main() {
    signal(SIGALRM, on_tick);
    struct itimerval t = {{1, 0}, {1, 0}};
    setitimer(ITIMER_REAL, &t, NULL);
    while (1) pause();
}

