#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void opt_read(int argc, char* argv[], int* i, int* f, int* v, int* num_key);

int file_to_file(char* r_file, char* w_file, int i, int f, int v);

int to_directory(int argc, char* argv[], int i, int f, int v, int num_key);

char* cr_path(char* a, char* b);

void buf_clean();

int main(int argc, char* argv[])
{
    //считывание ключей
    int i = 0, f = 0, v = 0, num_key = 0;
    opt_read(argc, argv, &i, &f, &v, &num_key);

    if (argc - num_key < 3)
    {
        printf("mycp: пропущен операнд, задающий файл.\n");
        return -1;
    }
    if (argc - num_key == 3)
        file_to_file(argv[argc - 2], argv[argc - 1], i, f, v);
    if (argc - num_key > 3)
        to_directory(argc, argv, i, f, v, num_key);

    return 0;
}

void opt_read(int argc, char* argv[], int* i, int* f, int* v, int* num_key)
{
    char opt;
    while ((opt = getopt(argc, argv, "ifv")) != -1)
        switch (opt)
        {
            case 'i':
                *i = 1;
                (*num_key)++;
                break;
            case 'f':
                *f = 1;
                (*num_key)++;
                break;
            case 'v':
                *v = 1;
                (*num_key)++;
                break;
        }
}

int file_to_file(char* r_file, char* w_file, int i, int f, int v)
{
    char buf[8192];

    int r_fd = open(r_file, O_RDONLY);
    if (r_fd == -1)
    {
        printf("mycp: невозможно открыть '%s' для чтения.\n", r_file);
        return -1;
    }
    int buf_size = read(r_fd, buf, sizeof(buf));
    close(r_fd);

    int w_fd = open(w_file, O_WRONLY);
    if (w_fd != -1 && i == 1)
    {
            printf("mycp: переписать '%s'?", w_file);
            char yes = getchar();
            buf_clean();
            if (yes != 'y' && yes != 'Y')
                return 0;
    }
    w_fd = open(w_file, O_WRONLY|O_TRUNC);
    if (w_fd == -1)
    {
        if (f == 1)
        {
            unlink(w_file);
            w_fd = open(w_file, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IROTH);
        }
        else
            w_fd = open(w_file, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IROTH);
        if (w_fd == -1)
        {
            printf("mycp: невозможно открыть файл '%s' на запись.\n", w_file);
            return -1;
        }
    }
    if (v == 1)
        printf("'%s' -> '%s'\n", r_file, w_file);
    write(w_fd, buf, buf_size);
    close(w_fd);
    return 0;
}

int to_directory(int argc, char* argv[], int i, int f, int v, int num_key)
{
    int w_fd = open(argv[argc - 1], O_DIRECTORY);
    if(w_fd == -1)
    {
        printf("mycp: указанная цель '%s' не является каталогом.\n", argv[argc - 1]);
        return -1;
    }

    for (int j = num_key + 1; j < argc - 1; j++)
    {
        char* path = cr_path(argv[argc - 1], argv[j]);
        file_to_file(argv[j], path, i, f, v);
        free(path);
    }

    return 0;
}

char* cr_path(char* a, char* b)
{
    char* path = (char*) malloc(8192 * sizeof(char));
    path[0] = '\0';
    strcat(path, a);
    strcat(path, "/");
    strcat(path, b);

    return path;
}

void buf_clean()
{
    while(getchar() != '\n');
}
