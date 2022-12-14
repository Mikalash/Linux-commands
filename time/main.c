#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

int main(int argc, char* argv[])
{
    struct timeval t_real_in, t_real_out;

    struct tms cl;

    int sec = 0;
    int usec = 0;

    int status;

    times(&cl);
    gettimeofday(&t_real_in, NULL);
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(argv[1], &argv[1]) == -1)
        {
            printf("%s: command not found\n", argv[1]);
            exit(-1);
        }
        exit(0);
    }
    wait(&status);
    times(&cl);
    gettimeofday(&t_real_out, NULL);

    printf("\n");

    struct timeval res;
    timersub(&t_real_out, &t_real_in, &res);
    printf("Real%5dm%d,%0.3ds\n", res.tv_sec / 60, res.tv_sec % 60, res.tv_usec / 1000);

    int user_m = ((int) cl.tms_cutime) / sysconf(_SC_CLK_TCK) / 60;
    float user_s = ((float) cl.tms_cutime) / sysconf(_SC_CLK_TCK) - user_m * 60;
    printf("User%5dm%0.3fs\n", user_m, user_s);

    int sys_m = ((int) cl.tms_cstime) / sysconf(_SC_CLK_TCK) / 60;
    float sys_s = ((float) cl.tms_cstime) / sysconf(_SC_CLK_TCK) - sys_m * 60;
    printf("Sys %5dm%0.3fs\n", sys_m, sys_s);
    return 0;
}
