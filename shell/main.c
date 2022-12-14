#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN_WSTR 1024
#define MAX_NUM_ARGV 64


void child(char* in_str, int fd);
void make_argv(char* in_str, int* argc, char* argv[]);

int main()
{
    while (1)
    {
        char* in_str = NULL;
        int n;

        printf("~$ ");
        if (getline(&in_str, &n, stdin) == -1)
            return 0;

        child(in_str, 0);

        free(in_str);
    }
}

void child(char* in_str, int fd)
{
    if (in_str == NULL)
        return;
    if (!strncmp(in_str, "exit\n", 5))
        exit(0);

    int fds[2];
    pipe(fds);

    int argc = 0;
    char* argv[MAX_LEN_WSTR];

    char w_str[MAX_LEN_WSTR];
    if (fd == 0)
        strcpy(w_str, strtok(in_str, "|"));
    else
    {
        char* to_w_str = strtok(NULL, "|");
        if (to_w_str != NULL)
            strcpy(w_str, to_w_str);
        else
            return;
    }

    int if_last_ch = 0;
    if (w_str[strlen(w_str) - 1] == '\n')
        if_last_ch = 1;

    make_argv(w_str, &argc, argv);

    pid_t pid = fork();
    if (pid == 0)
    {
        dup2(fd, 0);
        close(fd);

        if (!if_last_ch)
            dup2(fds[1], 1);

        if (execvp(argv[0], argv) == -1)
        {
            perror("shell");
            fprintf(stderr, "%s\n", w_str);
            exit(-1);
        }
    }

    wait(NULL);
    close(fds[1]);

    if (if_last_ch)
    {
        close(fds[0]);
        return;
    }

    child(in_str, fds[0]);
}

void make_argv(char* in_str, int* argc, char* argv[])
{
    char* save_ptr;

    argv[*argc] = strtok_r(in_str, " \t\n", &save_ptr);
    while(argv[*argc] != NULL && *argc < MAX_LEN_WSTR - 1)
    {
        (*argc)++;
        argv[*argc] = strtok_r(NULL, " \t\n", &save_ptr);
    }
}
