#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <signal.h>

const char* SHELL_PATH = "/TinyDOS/system32/tdsh";

void handle_shutdown_signal(int sig) {
    printf("\nInit: Shutdown signal received. Powering off.\n");
    sync();
    reboot(RB_POWER_OFF);
}

int main(int argc, char* argv[]) {
    if (getpid() != 1) {
        return 1;
    }

    signal(SIGTERM, handle_shutdown_signal);
    signal(SIGINT, handle_shutdown_signal);

    while (1) {
        pid_t pid = fork();

        if (pid < 0) {
            sleep(5);
            continue;
        }

        if (pid == 0) {
            char* const args[] = {(char*)SHELL_PATH, NULL};
            char* const envp[] = { "PATH=/TinyDOS/system32", NULL };
            execve(SHELL_PATH, args, envp);
            exit(1); 
        } else {
            waitpid(pid, NULL, 0);
            while(waitpid(-1, NULL, WNOHANG) > 0);
            sleep(2);
        }
    }
    return 0;
}
