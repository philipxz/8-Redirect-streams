#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return 1;
    }

    char *inp = argv[1];
    char *cmd = argv[2];
    char *out = argv[3];

    // Split the cmd string into separet arguments
    char *args[64];
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL && i < 63) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        if (strcmp(inp, "-") != 0) {
            int input_fd = open(inp, O_RDONLY);
            if (input_fd < 0) {
                perror("open inp");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (strcmp(out, "-") != 0) {
            int output_fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0) {
                perror("open out");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
