#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // child process: count forever
        int i = 0;
        while (1) {
            printf("child running: %d\n", i++);
            fflush(stdout);
            sleep(1);
        }
    }

    // parent process: control the child with signals
    sleep(3);
    printf("parent: STOP child\n");
    kill(pid, SIGSTOP);

    sleep(3);
    printf("parent: CONTINUE child\n");
    kill(pid, SIGCONT);

    sleep(3);
    printf("parent: KILL child\n");
    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
    return 0;
}
