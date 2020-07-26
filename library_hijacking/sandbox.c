#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

const char *arg[] = {"-d", "-p", "--"};
int arg_num = 3, sh_len = 0;
char *dir = NULL, *path = NULL;
char **proc_argv = NULL;
char *sh_argv[4] = {"sh", "-c", NULL, NULL};

void usage (char *erorr_arg) {
    if (erorr_arg != NULL)
        printf ("./sandbox: invalid option -- '%s'\n", erorr_arg + 1);

    printf ("usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n");
    printf ("       -p: set the path to sandbox.so, default = ./sandbox.so\n");
    printf ("       -d: the base directory that is allowed to access, default = .\n");
    printf ("       --: seperate the arguments for sandbox and for the executed command\n");

    exit (-1);
}

int set_proc_arg (int start, int argc, char **argv, int parse_all) {
    int j = 1;
    proc_argv = malloc (sizeof (char *) * (argc + 1));
    while (start < argc && (parse_all || argv[start][0] != '-')) {
        sh_len += strlen (argv[start]) + 1;
        proc_argv[j++] = argv[start++];
    }
    proc_argv[j] = NULL;

    return start - 1;
}

void readarg (int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int flag = -1;
            for (int j = 0; j < arg_num; j++)
                if (!strcmp (argv[i], arg[j])) {
                    flag = j;
                    break;
                }

            switch (flag) {
                case 0:
                    if (++i >= argc || argv[i][0] == '-')
                        usage (NULL);
                    if (dir == NULL)
                        dir = argv[i];
                    break;
                case 1:
                    if (++i >= argc || argv[i][0] == '-')
                        usage (NULL);
                    if (path == NULL)
                        path = argv[i];
                    break;
                case 2:
                    if (++i >= argc || proc_argv != NULL)
                        usage (NULL);
                    set_proc_arg (i, argc, argv, 1);
                    return;
                default:
                    usage (argv[i]);
            }
        }
        else {
            if (proc_argv != NULL)
                usage (NULL);
            i = set_proc_arg (i, argc, argv, 0);
        }
    }

    if (proc_argv == NULL)
        usage (NULL);
}

void debug () {
    printf ("dir:\t%s\n", dir);
    printf ("path:\t%s\n", path);
    printf ("arg:\t");
    for (int i = 0; proc_argv[i] != NULL; i++)
        printf ("%s ", proc_argv[i]);
    printf ("\n");
    printf ("%s %s %s\n", sh_argv[0], sh_argv[1], sh_argv[2]);
}

int main (int argc, char **argv) {
    readarg (argc, argv);

    if (dir == NULL)
        dir = ".";
    if (path == NULL)
        path = "./sandbox.so";

    char *real_dir = realpath (dir, NULL);
    char *real_path = realpath (path, NULL);

    char real_argv[strlen (real_path) + strlen (real_dir) + 50];
    snprintf (real_argv, sizeof (real_argv), "LD_PRELOAD=%s ROOT_DIR=%s", real_path, real_dir);
    proc_argv[0] = real_argv;
    
    sh_argv[2] = malloc (sizeof (char) * (sh_len + 20) + sizeof (real_argv));
    sprintf (sh_argv[2], "%s", proc_argv[0]);
    for (int i = 1; proc_argv[i] != NULL; i++)
        sprintf (sh_argv[2], "%s %s", sh_argv[2], proc_argv[i]);

    // if (chdir (dir) == -1) {
    //     printf ("Changing base directory to %s failed.\n", dir);
    //     exit (-1);
    // }

    // debug ();

    execv ("/bin/sh", sh_argv);
}