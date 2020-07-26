#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_STRING 1024

int writen (int fd, char *ptr, int n)
{
    int nleft = n, nwritten;
    char *ptr_tmp = ptr;

    while (nleft > 0)
    {
        if ((nwritten = write (fd, ptr_tmp, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return (-1);
        }

        nleft -= nwritten;
        ptr_tmp += nwritten;
    }

    return (n);
}

int readline (int fd, char *ptr, int maxlen)
{
    int n = 1, rc;
    char c, *ptr_tmp = ptr;

    while (n < maxlen)
    {
        if ((rc = read (fd, &c, 1)) == 1)
        {
            *ptr_tmp = c;
            ptr_tmp++;
            if (c == '\n')
                break;
        }
        else if (rc == 0)
        {
            *ptr_tmp = 0;
            return (n - 1);
        }
        else
        {
            if (errno == EINTR)
                continue;
            return (-1);
        }
        n++;
    }

    *ptr_tmp = 0;
    return (n);
}

void client (int sock_fd)
{
    int maxfdp1, stdineof;
    fd_set rset;
    char send [MAX_STRING], recv [MAX_STRING];

    stdineof = 0;
    FD_ZERO (&rset);
    while (1)
    {
        if (stdineof == 0)
            FD_SET (0, &rset);
        FD_SET (sock_fd, &rset);
        maxfdp1 = sock_fd + 1;
        select (maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET (sock_fd, &rset))
        {
            if (readline (sock_fd, recv, MAX_STRING) == 0)
            {
                if (stdineof == 1)
                    return;
                else
                {
                    printf ("client: server terminated prematurely\n");
                    exit (-1);
                }
            }

            fputs (recv, stdout);
        }

        if (FD_ISSET (0, &rset))
        {
            if (fgets (send, MAX_STRING, stdin) == 0 || !strncmp (send, "exit\n", strlen (send)))
            {
                stdineof = 1;
                shutdown (sock_fd, SHUT_WR);
                FD_CLR (0, &rset);
                continue;
            }

            writen (sock_fd, send, strlen (send));
        }
    }
}

int main (int argc, char **argv)
{
    if (argc != 3)
    {
        printf ("[Usage] ./client [server ip] [server port]\n");
        exit (-1);
    }

    int sock_fd;
    socklen_t len;
    struct sockaddr_in servaddr;

    sock_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        printf ("Fail to create socket!\n");
        exit (-1);
    }

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons (atoi (argv [2]));
    servaddr.sin_addr.s_addr = inet_addr (argv [1]);

    connect (sock_fd, (struct sockaddr *) &servaddr, sizeof (servaddr));

    client (sock_fd);
}
