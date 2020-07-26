#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/types.h>

#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h>

#define AT_FDCWD		-100

/*-----macro for linking library-----*/
#define DLINK(func) ({  \
    if (real_##func == NULL)    \
        real_##func = dlsym (RTLD_NEXT, #func); \
    real_##func;    \
})  \
/*-----------------------------------*/

typedef struct __dirstream DIR;
typedef struct stat st;
FILE *fp = NULL;
static char *root = NULL;

/*-----functions-----*/
int chdir (const char *);
int chmod (const char *, __mode_t);
int chown (const char *, __uid_t, __gid_t);
int creat (const char *, mode_t);
int creat64 (const char *, mode_t);
FILE *fopen (const char *, const char *);
FILE *fopen64 (const char *, const char *);
int link (const char *, const char *);
int mkdir (const char *, __mode_t);
int open (const char *, int, ...);
int open64 (const char *, int, ...);
int openat (int, const char *, int, ...);
int openat64 (int, const char *, int, ...);
DIR *opendir (const char *);
ssize_t readlink (const char *, char *, size_t);
int remove (const char *);
int rename (const char *, const char *);
int rmdir (const char *);
int __xstat (int, const char *, st *);
int __xstat64 (int, const char *, st *);
int symlink (const char *, const char *);
int unlink (const char *);

int execl (const char *, const char *, ...);
int execle (const char *, const char *, ...);
int execlp (const char *, const char *, ...);
int execv (const char *, char *const []);
int execvp (const char *, char *const []);
int execve (const char *, char *const [], char *const []);
int system (const char *);
/*-------------------*/


/*-----real system calls-----*/
static int (*real_chdir) (const char *);
static int (*real_chmod) (const char *, __mode_t);
static int (*real_chown) (const char *, __uid_t, __gid_t);
static int (*real_creat) (const char *, mode_t);
static int (*real_creat64) (const char *, mode_t);
static FILE *(*real_fopen) (const char * , const char *);
static FILE *(*real_fopen64) (const char * , const char *);
static int (*real_link) (const char *, const char *);
static int (*real_mkdir) (const char *, __mode_t);
static int (*real_open) (const char *, int, ...);
static int (*real_open64) (const char *, int, ...);
static int (*real_openat) (int, const char *, int, ...);
static int (*real_openat64) (int, const char *, int, ...);
static DIR *(*real_opendir) (const char *);
static ssize_t (*real_readlink) (const char *, char *, size_t);
static int (*real_remove) (const char *);
static int (*real_rename) (const char *, const char *);
static int (*real_rmdir) (const char *);
static int (*real___xstat) (int, const char *, st *);
static int (*real___xstat64) (int, const char *, st *);
static int (*real_symlink) (const char *, const char *);
static int (*real_unlink) (const char *);
/*---------------------------*/


/*-----for checking the given path is allowed or not-----*/
// int check_path_legal (char *func, const char *path, int chroot) {
//     char buf[PATH_MAX];

//     if (fp == NULL)
//         fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

//     realpath (path, buf);
//     if (root == NULL) {
//         root = getenv ("ROOT_DIR");
//     }

//     int buf_len = strlen (buf), root_len = strlen (root);
//     if (chroot && strcmp (buf, root) == 0 && buf_len == root_len)
//         return 1;

//     if (strcmp ("/", root) == 0 || (strncmp (root, buf, root_len) == 0 && buf[root_len] == '/'))
//         return 1;
    
//     fprintf (fp, "[sandbox] %s: access to %s is not allowed\n", func, buf);
//     return 0;
// }

int check_path_legal_no_link (char *func, const char *path, int chroot) {
    char buf[PATH_MAX], path_t[PATH_MAX];

    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    if (root == NULL) {
        root = getenv ("ROOT_DIR");
    }

    strcpy (path_t, path);

    int slash[PATH_MAX];
    int now = -1;

    if (path_t[0] != '/') {
        getcwd (buf, sizeof (buf));
        for (int i = 0; i < strlen (buf); i++)
            if (buf[i] == '/')
                slash[++now] = i;
    }
    else {
        buf[0] = '/'; buf[1] = '\0';
        slash[0] = 0;
    }

    char *tmp = strtok (path_t, "/");
    if (tmp != NULL)
        do {
            if (strcmp (tmp, ".") == 0)
                continue;

            if (strcmp (tmp, "..") == 0) {
                if (now > 0)
                    buf[slash[now--]] = '\0';
                else {
                    buf[1] = '\0';
                    now = -1;
                }
                continue;
            }

            if (now == -1) {
                strcat (buf, tmp);
                now++;
                continue;
            }

            slash[++now] = strlen (buf);
            sprintf (buf, "%s/%s", buf, tmp);
        } while (tmp = strtok (NULL, "/"));

    int buf_len = strlen (buf), root_len = strlen (root);
    if (chroot && strcmp (buf, root) == 0 && buf_len == root_len)
        return 1;

    if (strcmp ("/", root) == 0 || (strncmp (root, buf, root_len) == 0 && buf[root_len] == '/'))
        return 1;
    
    fprintf (fp, "[sandbox] %s: access to %s is not allowed\n", func, buf);
    return 0;
}
/*-------------------------------------------------------*/

void print_cwd (char *func) {
    char cwd[PATH_MAX];
    printf ("%s\n%s\n", func, getcwd (cwd, sizeof (cwd)));
}

/*-----path limited functions-----*/
int chdir (const char *__path) {
    if (check_path_legal_no_link ("chdir", __path, 1))
        return DLINK (chdir) (__path);
    return -1;
}

int chmod (const char *__file, __mode_t __mode) {
    if (check_path_legal_no_link ("chmod", __file, 0))
        return DLINK (chmod) (__file, __mode);
    return -1;
}

int chown (const char *__file, __uid_t __owner, __gid_t __group) {
    if (check_path_legal_no_link ("chown", __file, 0))
        return DLINK (chown) (__file, __owner, __group);
    return -1;
}

int creat (const char *__file, mode_t __mode) {
    if (check_path_legal_no_link ("creat", __file, 0))
        return DLINK (creat) (__file, __mode);
    return -1;
}

int creat64 (const char *__file, mode_t __mode) {
    if (check_path_legal_no_link ("creat64", __file, 0))
        return DLINK (creat64) (__file, __mode);
    return -1;
}

FILE *fopen (const char *__filename, const char *__modes) {
    if (check_path_legal_no_link ("fopen", __filename, 1))
        return DLINK (fopen) (__filename, __modes);
    return NULL;
}

FILE *fopen64 (const char *__filename, const char *__modes) {
    if (check_path_legal_no_link ("fopen64", __filename, 1))
        return DLINK (fopen64) (__filename, __modes);
    return NULL;
}

int link (const char *__from, const char *__to) {
    if (check_path_legal_no_link ("link", __from, 0) && check_path_legal_no_link ("link", __to, 0))
        return DLINK (link) (__from, __to);
    return -1;
}

int mkdir (const char *__path, __mode_t __mode) {
    if (check_path_legal_no_link ("mkdir", __path, 0))
        return DLINK (mkdir) (__path, __mode);
    return -1;
}

int open (const char *__path, int __oflag, ...) {
    if (!check_path_legal_no_link ("open", __path, 1))
        return -1;

    va_list arg;

    va_start (arg, __oflag);
    int fd = DLINK (open) (__path, __oflag, (int) va_arg (arg, int));
    va_end (arg);

    return fd;
}

int open64 (const char *__path, int __oflag, ...) {
    if (!check_path_legal_no_link ("open64", __path, 1))
        return -1;

    va_list arg;

    va_start (arg, __oflag);
    int fd = DLINK (open64) (__path, __oflag, (int) va_arg (arg, int));
    va_end (arg);

    return fd;
}

int openat(int __dirfd, const char *__path, int flags, ...) {
    char path[PATH_MAX], cwd[PATH_MAX], path_t[PATH_MAX];

    if (__dirfd != AT_FDCWD) {
        getcwd (cwd, sizeof (cwd));
        if (fchdir (__dirfd)) return -1;
        getcwd (path, sizeof (path));
        DLINK (chdir) (cwd);
    }
    else
        getcwd (path, sizeof (path));

    sprintf (path_t, "%s/%s", path, __path);
    if (!check_path_legal_no_link ("openat", path, 1) || !check_path_legal_no_link ("openat", path_t, 1))
        return -1;

    va_list arg;

    va_start (arg, flags);
    int fd = DLINK (openat) (__dirfd, __path, flags, (mode_t) va_arg (arg, mode_t));
    va_end (arg);

    return fd;
}

int openat64(int __dirfd, const char *__path, int flags, ...) {
    char path[PATH_MAX], cwd[PATH_MAX], path_t[PATH_MAX];

    if (__dirfd != AT_FDCWD) {
        getcwd (cwd, sizeof (cwd));
        if (fchdir (__dirfd)) return -1;
        getcwd (path, sizeof (path));
        DLINK (chdir) (cwd);
    }
    else
        getcwd (path, sizeof (path));

    sprintf (path_t, "%s/%s", path, __path);
    if (!check_path_legal_no_link ("openat64", path, 1) || !check_path_legal_no_link ("openat64", path_t, 1))
        return -1;

    va_list arg;

    va_start (arg, flags);
    int fd = DLINK (openat64) (__dirfd, __path, flags, (mode_t) va_arg (arg, mode_t));
    va_end (arg);

    return fd;
}

DIR *opendir (const char *__name) {
    if (check_path_legal_no_link ("opendir", __name, 1))
        return DLINK (opendir) (__name);
    return NULL;
}

ssize_t readlink(const char *path, char *buf, size_t size) {
    if (check_path_legal_no_link ("readlink", path, 1))
        return DLINK (readlink) (path, buf, size);
    return -1;
}

int remove (const char *pathname) {
    if (check_path_legal_no_link ("remove", pathname, 0))
        return DLINK (remove) (pathname);
    return -1;
}

int rename (const char *__old, const char *__new) {
    if (check_path_legal_no_link ("rename", __old, 0))
        return DLINK (rename) (__old, __new);
    return -1;
}

int rmdir (const char *__path) {
    if (check_path_legal_no_link ("rmdir", __path, 0))
        return DLINK (rmdir) (__path);
    return -1;
}

int __xstat (int vers, const char *__file, st *__buf) {
    if (check_path_legal_no_link ("__xstat", __file, 1))
        return DLINK (__xstat) (vers, __file, __buf);
    return -1;
}

int __xstat64 (int vers, const char *__file, st *__buf) {
    if (check_path_legal_no_link ("__xstat64", __file, 1))
        return DLINK (__xstat64) (vers, __file, __buf);
    return -1;
}

int symlink (const char *path1, const char *path2) {
    if (check_path_legal_no_link ("symlink", path1, 1) && check_path_legal_no_link ("symlink", path2, 0))
        return DLINK (symlink) (path1, path2);
    return -1;
}

int unlink (const char *__name) {
    if (check_path_legal_no_link ("unlink", __name, 0))
        return DLINK (unlink) (__name);
    return -1;
}
/*--------------------------------*/


/*-----always not allow functions-----*/
int execl (const char *__path, const char *__arg, ...) {
    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    va_list argv;
    char *arg;

    va_start (argv, __arg);
    fprintf (fp, "[sandbox] execl(%s, %s", __path, __arg);

    while (arg = (char *) va_arg (argv, const char *))
        fprintf (fp, ", %s", arg);

    fprintf (fp, ", NULL): not allowed\n");

    va_end (argv);
}

int execle (const char *__path, const char *__arg, ...) {
    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    va_list argv;
    char *arg, **__argv;

    va_start (argv, __arg);
    fprintf (fp, "[sandbox] execle(%s, %s", __path, __arg);

    while (arg = (char *) va_arg (argv, const char *))
        fprintf (fp, ", %s", arg);

    fprintf (fp, ", NULL");

    if ((__argv = (char **) va_arg (argv, const char **)) == NULL)
        fprintf (fp, ", NULL");
    else
        // for (int i = 0; arg = (char *) __argv[i]; i++)
        //     printf (", %s", arg);
        fprintf (fp, ", %p", __argv);

    fprintf (fp, "): not allowed\n");

    va_end (argv);
}

int execlp (const char *__file, const char *__arg, ...) {
    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    va_list argv;
    char *arg;

    va_start (argv, __arg);
    fprintf (fp, "[sandbox] execlp(%s, %s", __file, __arg);

    while (arg = (char *) va_arg (argv, const char *))
        fprintf (fp, ", %s", arg);

    fprintf (fp, ", NULL): not allowed\n");

    va_end (argv);
}

int execv (const char *__path, char *const __argv[]) {
    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    char *arg;

    fprintf (fp, "[sandbox] execv(%s", __path);

    // for (int i = 0; arg = (char *) __argv[i]; i++)
    //     fprintf (fp, ", %s", arg);
    fprintf (fp, ", %p", __argv);

    fprintf (fp, "): not allowed\n");
}

int execvp (const char *__file, char *const __argv[]) {
    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    char *arg;

    fprintf (fp, "[sandbox] execvp(%s", __file);

    // for (int i = 0; arg = (char *) __argv[i]; i++)
    //     fprintf (fp, ", %s", arg);
    fprintf (fp, ", %p", __argv);

    fprintf (fp, "): not allowed\n");
}

int execve (const char *__path, char *const __argv[], char *const __envp[]) {
    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    char *arg;

    fprintf (fp, "[sandbox] execve(%s", __path);

    if (__argv == NULL)
        fprintf (fp, ", NULL");
    else
        // for (int i = 0; arg = (char *) __argv[i]; i++)
        //     fprintf (fp, ", %s", arg);
        fprintf (fp, ", %p", __argv);

    if (__envp == NULL)
        fprintf (fp, ", NULL");
    else
        // for (int i = 0; arg = (char *) __envp[i]; i++)
        //     fprintf (fp, ", %s", arg);
        fprintf (fp, ", %p", __argv);

    fprintf (fp, "): not allowed\n");
}

int system (const char *__command) {
    if (fp == NULL)
        fp = DLINK (fopen) (ttyname (STDIN_FILENO), "w");

    fprintf (fp, "[sandbox] system(%s): not allowed\n", __command);
}
/*------------------------------------*/