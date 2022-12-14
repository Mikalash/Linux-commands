#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#include <fcntl.h>
#include <dirent.h>

#include <pwd.h>
#include <grp.h>

#define SHOW_NAME 1

struct opt
{
    int l;
    int d;
    int a;
    int R;
    int i;
    int n;
};
struct opt in_opt;
void opt_read(int argc, char* argv[]);

void first_out(int argc, char* argv[]);
void file_show(char file_path[], char file_name[]);
void file_lshow(char file_path[], char file_name[]);
void directory_show(char a[], int show_name);
int directory_total(char a[]);
void recurs(char a[]);

char* cr_path(char* a, char* b);
void swapp(char* a[], char* b[]);
int sort_f_d(int argc, char* argv[]);
void sort_crack_f(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    opt_read(argc, argv);

    if (in_opt.n)
        in_opt.l = 1;
    if (optind == argc)
    {
        optind--;
        argv[optind] = ".";
    }

    first_out(argc - optind, &argv[optind]);

    return 0;
}

void opt_read(int argc, char* argv[])
{
    char opt;
    while ((opt = getopt(argc, argv, "ldaRin")) != -1)
        switch (opt)
        {
            case 'l':
                in_opt.l = 1;
                break;
            case 'd':
                in_opt.d = 1;
                break;
            case 'a':
                in_opt.a = 1;
                break;
            case 'R':
                in_opt.R = 1;
                break;
            case 'i':
                in_opt.i = 1;
                break;
            case 'n':
                in_opt.n = 1;
                break;
        }
}

void first_out(int argc, char* argv[])
{
    int num_f =  sort_f_d(argc, argv);
    if (in_opt.d)
        num_f = argc;
    if (argc == 1 && !num_f)
    {
        directory_show(argv[0], !SHOW_NAME);
        return;
    }
    sort_crack_f(num_f, argv);

    for (int i = 0; i < num_f; i++)
        file_show(argv[i], argv[i]);

    if (!(!(argc - num_f) && in_opt.l) && num_f)
    {
        printf("\n");
        if ((argc - num_f) && !in_opt.l)
            printf("\n");
    }

    for (int i = num_f; i < argc; i++)
    {
        if (i > num_f)
            printf("\n");
        directory_show(argv[i], SHOW_NAME);
    }
}

void file_show(char file_path[], char file_name[])
{
    struct stat sb;
    if (lstat(file_path, &sb) == -1)
    {
        printf("myls: unable to access file '%s'\n", file_name);
        return;
    }

    if (in_opt.i)
        printf("%ju ", sb.st_ino);
    if (in_opt.l)
            file_lshow(file_path, file_name);
    else
        printf("%s ", file_name);
}

void file_lshow(char file_path[], char file_name[])
{
    struct stat sb;
    lstat(file_path, &sb);
    //file type
    printf ("%c", (sb.st_mode & S_IFREG) ? '-'  : '\0');
    printf ("%c", (sb.st_mode & S_IFDIR) ? 'd'  : '\0');
    printf ("%c", (sb.st_mode & S_IFCHR) ? 'c'  : '\0');
    printf ("%c", (sb.st_mode & S_IFIFO) ? 'f'  : '\0');
    //access rights
    printf ("%c", (sb.st_mode & S_IRUSR) ? 'r' : '-');
    printf ("%c", (sb.st_mode & S_IWUSR) ? 'w' : '-');
    printf ("%c", (sb.st_mode & S_IXUSR) ? 'x' : '-');

    printf ("%c", (sb.st_mode & S_IRGRP) ? 'r' : '-');
    printf ("%c", (sb.st_mode & S_IWGRP) ? 'w' : '-');
    printf ("%c", (sb.st_mode & S_IXGRP) ? 'x' : '-');

    printf ("%c", (sb.st_mode & S_IROTH) ? 'r' : '-');
    printf ("%c", (sb.st_mode & S_IWOTH) ? 'w' : '-');
    printf ("%c", (sb.st_mode & S_IXOTH) ? 'x' : '-');
    printf (" ");
    //number of links
    printf("%-2ju ", (uintmax_t) sb.st_nlink);
    //owners name
    if (in_opt.n)
        printf("%ju ", sb.st_uid);
    else
        printf("%s ", getpwuid(sb.st_uid)->pw_name);
    //group name
    if (in_opt.n)
        printf("%ju ", sb.st_gid);
    else
        printf("%s ", getgrgid(sb.st_gid)->gr_name);
    //file size
    printf("%-5jd ", (intmax_t) sb.st_size);
    //last modification time
    char time_last[1024];
    strcpy(time_last, ctime(&sb.st_mtim));
    time_last[16] = '\0';
    printf("%s ", &time_last[4]);
    //file name
    printf("%s\n", file_name);
}

void directory_show(char a[], int show_name)
{
    if (show_name)
        printf("%s:\n", a);
    if (in_opt.l)
        printf("total %ld\n", directory_total(a)/2);

    DIR* current_dir = opendir(a);
    struct dirent* current_dirent = readdir(current_dir);

    int file_in_dir = 0;
    while(current_dirent != NULL)
    {
        if ((current_dirent->d_name)[0] != '.' ||  in_opt.a)
        {
            char* file_name = current_dirent->d_name;
            char* file_path = cr_path(a, file_name);
            file_show(file_path, file_name);
            file_in_dir = 1;
            free(file_path);
        }
        current_dirent = readdir(current_dir);
    }

    if (!in_opt.l && file_in_dir)
        printf("\n");

    closedir(current_dir);
    free(current_dirent);

    if (in_opt.R)
        recurs(a);
}

int directory_total(char a[])
{
    int total = 0;

    DIR* current_dir = opendir(a);
    struct dirent* current_dirent = readdir(current_dir);

    while(current_dirent != NULL)
    {
        if ((current_dirent->d_name)[0] != '.' ||  in_opt.a)
        {
            char* file_name = current_dirent->d_name;
            char* file_path = cr_path(a, file_name);
            struct stat sb;
            lstat(file_path, &sb);

            total += sb.st_blocks;
            free(file_path);
        }
        current_dirent = readdir(current_dir);
    }
    closedir(current_dir);
    free(current_dirent);

    return total;
}

void recurs(char a[])
{
    DIR* current_dir = opendir(a);
    struct dirent* current_dirent = readdir(current_dir);

    while(current_dirent != NULL)
    {
        if (strcmp(".", current_dirent->d_name) != 0 && strcmp("..", current_dirent->d_name) != 0)
        {
            char* file_name = current_dirent->d_name;
            char* file_path = cr_path(a, file_name);

            struct stat sb;
            lstat(file_path, &sb);

            DIR* c_dir = opendir(file_path);
            if (c_dir == NULL && S_ISDIR(sb.st_mode))
            {
                printf("myls: unable to access directory '%s'\n", file_path);
                closedir(c_dir);
                free(file_path);
                current_dirent = readdir(current_dir);
                continue;
            }
            closedir(c_dir);

            if (S_ISDIR(sb.st_mode))
                if ((file_name)[0] != '.' ||  in_opt.a)
                {
                    printf("\n");
                    directory_show(file_path, SHOW_NAME);
                }
            free(file_path);
        }
        current_dirent = readdir(current_dir);
    }

    closedir(current_dir);
    free(current_dirent);
}

char* cr_path(char* a, char* b)
{
    char* path = (char*) malloc(16384 * sizeof(char));
    path[0] = '\0';
    strcat(path, a);
    if (path[strlen(path) - 1] != '/')
        strcat(path, "/");
    strcat(path, b);

    return path;
}

void swapp(char* a[], char* b[])
{
    char* c;
    c = *a;
    *a = *b;
    *b = c;
}

int sort_f_d(int argc, char* argv[])
{
    int i = 0;
    argc--;
    while (argc - i >= 0)
    {
        DIR* current_dir = opendir(argv[i]);
        if (current_dir == NULL)
        {
            closedir(current_dir);
            i++;
            continue;
        }
        DIR* current_fil = opendir(argv[argc]);
        if (current_fil != NULL)
        {
            closedir(current_dir);
            closedir(current_fil);
            argc--;
            continue;
        }
        swapp(&argv[i], &argv[argc]);
        closedir(current_dir);
        closedir(current_fil);
    }

    return i;
}

void sort_crack_f(int argc, char* argv[])
{
    int i = 0;
    argc--;
    while (argc - i >= 0)
    {
        struct stat sb_l;
        if (lstat(argv[i], &sb_l) == -1)
        {
            i++;
            continue;
        }
        struct stat sb_r;
        if (lstat(argv[argc], &sb_r) != -1)
        {
            argc--;
            continue;
        }
        swapp(&argv[i], &argv[argc]);
    }
}
