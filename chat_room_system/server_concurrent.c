#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_STRING 1024
#define MAX_USER 10
#define KEY_NUM 5

char key [5][10] = {"who", "name", "tell", "yell", "ban"};
char anonymous [10] = {"anonymous"};

struct User_data
{
    int fd;
    char name [15];
    char ip [INET_ADDRSTRLEN];
    int port;
    int banned [10];
} user_data [MAX_USER];

void sig_handler (int sig_num)
{
    if (sig_num == SIGINT)
    {
        printf ("server closed\n");
        exit (0);
    }
}

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

int my_strcmp (char *str1, char *str2)
{
    if (!strncmp (str1, str2, strlen (str1)) && !strncmp (str1, str2, strlen (str2)))
        return 1;
    return 0;
}

int not_english (char *name)
{
    int len = strlen (name), flag = 0;
    char c;
    for (int i = 0; i < len; i++)
    {
        c = name [i];
        if (c < 'A' || (c > 'Z' && c < 'a') || c > 'z')
            return 1;
    }
    return 0;
}

void hello (int user_id)
{
    char send [MAX_STRING];

    printf ("Anonymous from %s:%d\n", user_data [user_id].ip, user_data [user_id].port);

    for (int i = 0; i < MAX_USER; i++)
    {
        if (user_data [i].fd != -1)
        {
            if (i == user_id)
                snprintf (send, MAX_STRING, "[Server] Hello, anonymous! From: %s:%d\n", user_data [i].ip, user_data [i].port);
            else
                snprintf (send, MAX_STRING, "[Server] Someone is coming!\n");

            writen (user_data [i].fd, send, strlen (send));
        }
    }
}

void offline (int user_id)
{
    char send [MAX_STRING];
    
    printf ("%s is offline.\n", user_data [user_id].name);

    for (int i = 0; i < MAX_USER; i++)
    {
        if (user_data [i].fd != -1)
        {
            snprintf (send, MAX_STRING, "[Server] %s is offline.\n", user_data [user_id].name);
            writen (user_data [i].fd, send, strlen (send));
        }
    }
}

void who (int user_id)
{
    char send [MAX_STRING];

    printf ("%s calls who.\n", user_data [user_id].name);

    for (int i = 0; i < MAX_USER; i++)
    {
        if (user_data [i].fd != -1)
        {
            if (i == user_id)
                snprintf (send, MAX_STRING, "[Server] %s %s:%d ->me\n", user_data [i].name, user_data [i].ip, user_data [i].port);
            else
                snprintf (send, MAX_STRING, "[Server] %s %s:%d\n", user_data [i].name, user_data [i].ip, user_data [i].port);

            writen (user_data [user_id].fd, send, strlen (send));
        }
    }
}

void change_name (int user_id, char *newname)
{
    char send [MAX_STRING];

    printf ("%s changes name.\n", user_data [user_id].name);

    if (strlen (newname) > 12 || strlen (newname) < 2 || not_english (newname))
    {
        snprintf (send, MAX_STRING, "[Server] ERROR: Username can only consists of 2~12 English letters.\n");
        writen (user_data [user_id].fd, send, strlen (send));
        return;
    }

    if (my_strcmp (newname, anonymous))
    {
        snprintf (send, MAX_STRING, "[Server] ERROR: Username cannot be anonymous.\n");
        writen (user_data [user_id].fd, send, strlen (send));
        return;
    }

    for (int i = 0; i < MAX_USER; i++)
    {
        if (user_data [i].fd != -1)
        {
            if (i != user_id && my_strcmp (user_data [i].name, newname))
            {
                snprintf (send, MAX_STRING, "[Server] ERROR: %s has been used by others.\n", newname);
                writen (user_data [user_id].fd, send, strlen (send));
                return; 
            }
        }
    }

    for (int i = 0; i < MAX_USER; i++)
    {
        if (user_data [i].fd != -1)
        {
            if (i == user_id)
                snprintf (send, MAX_STRING, "[Server] You're now known as %s.\n", newname);
            else
                snprintf (send, MAX_STRING, "[Server] %s is now known as %s.\n", user_data [user_id].name, newname);
            
            writen (user_data [i].fd, send, strlen (send));
        }
    }

    strncpy (user_data [user_id].name, newname, sizeof (user_data[user_id].name));
}

void private_msg (int user_id, char *send_to, char *msg)
{
    char send [MAX_STRING];

    printf ("%s calls tell.\n", user_data [user_id].name);

    if (my_strcmp (anonymous, user_data [user_id].name))
    {
        snprintf (send, MAX_STRING, "[Server] ERROR: You are anonymous.\n");
        writen (user_data [user_id].fd, send, strlen (send));
        return;
    }

    if (my_strcmp (anonymous, send_to))
    {
        snprintf (send, MAX_STRING, "[Server] ERROR: The client to which you sent is anonymous.\n");
        writen (user_data [user_id].fd, send, strlen (send));
        return;
    }

    for (int i = 0; i < MAX_USER; i++)
    {
        if (my_strcmp (user_data [i].name, send_to))
        {
            if (user_data [user_id].banned [i] == 1)
            {
                snprintf (send, MAX_STRING, "[Server] You are banned by %s.\n", user_data [i].name);
                writen (user_data [user_id].fd, send, strlen (send));
                return;
            }
            snprintf (send, MAX_STRING, "[Server] SUCCESS: Your message has been sent.\n");
            writen (user_data [user_id].fd, send, strlen (send));
            snprintf (send, MAX_STRING, "[Server] %s tell you %s\n", user_data [user_id].name, msg);
            writen (user_data [i].fd, send, strlen (send));
            return;
        }
    }

    snprintf (send, MAX_STRING, "[Server] ERROR: The recevier doesn't exist.\n");
    writen (user_data [user_id].fd, send, strlen (send));
}

void yell (int user_id, char *msg)
{
    char send [MAX_STRING];

    printf ("%s calls yell.\n", user_data [user_id].name);

    for (int i = 0; i < MAX_USER; i++)
    {
        if (user_data [i].fd != -1)
        {
            if (user_data [user_id].banned [i] == 1)
            {
                snprintf (send, MAX_STRING, "[Server] You are banned by %s.\n", user_data [i].name);
                writen (user_data [user_id].fd, send, strlen (send));
                continue;
            }
            snprintf (send, MAX_STRING, "[Server] %s yell %s\n", user_data [user_id].name, msg);
            writen (user_data [i].fd, send, strlen (send));
        }
    }
}

void ban (int user_id, char *banned_name)
{
    char send [MAX_STRING];

    for (int i = 0; i < MAX_USER; i++)
    {
        if (my_strcmp (user_data [i].name, banned_name))
        {
            user_data [i].banned [user_id] = 1;
            user_data [user_id].banned [i] = 1;
            
            snprintf (send, MAX_STRING, "[Server] You have banned %s.\n", user_data [i].name);
            writen (user_data [user_id].fd, send, strlen (send));

            snprintf (send, MAX_STRING, "[Server] You are banned by %s.\n", user_data [user_id].name);
            writen (user_data [i].fd, send, strlen (send));

            return;
        }
    }
}

void error (int user_id)
{
    char send [MAX_STRING];

    snprintf (send, MAX_STRING, "[Server] ERROR: Error command.\n");
    printf (send);
    writen (user_data [user_id].fd, send, strlen (send));
}

void read_cmd (int user_id, char *recv)
{
    char *cmd = strtok (recv, " \n");
    char *arg = strtok (NULL, "\n");
    char *name, *msg;

    int key_id = -1;
    for (int i = 0; i < KEY_NUM; i++)
    {
        if (my_strcmp (cmd, key [i]))
        {
            key_id = i;
            break;
        }
    }

    switch (key_id)
    {
        case 0:
            who (user_id);
            break;
        case 1:
            if (!arg)
                error (user_id);
            else
                change_name (user_id, arg);
            break;
        case 2:
            name = strtok (arg, " ");
            msg = strtok (NULL, "");

            if (!name || !msg)
                error (user_id);
            else
                private_msg (user_id, name, msg);
            break;
        case 3:
            if (!arg)
                error (user_id);
            else
                yell (user_id, arg);
            break;
        case 4:
            if (!arg)
                error (user_id);
            else
                ban (user_id, arg);
            break;
        default:
            error (user_id);
            break;
    }
}

void server (int listen_fd)
{
    int maxfd = listen_fd, maxi = -1, sock_fd, conn_fd, ready;
    char send [MAX_STRING], recv [MAX_STRING];
    struct sockaddr_in cliaddr;
    for (int i = 0; i < MAX_USER; i++)
        user_data [i].fd = -1;

    fd_set allset, rset;
    FD_ZERO (&allset);
    FD_SET (listen_fd, &allset);
    while (1)
    {
        rset = allset;
        ready = select (maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET (listen_fd, &rset))
        {
            int clilen = sizeof (cliaddr);
            conn_fd = accept (listen_fd, (struct sockaddr *) &cliaddr, &clilen);

            int i;
            for (i = 0; i < MAX_USER; i++)
            {
                if (user_data [i].fd < 0)
                {
                    user_data [i].fd = conn_fd;

                    strncpy (user_data [i].name, "anonymous", sizeof (user_data [i].name));
                    
                    char ip [INET_ADDRSTRLEN];
                    inet_ntop (AF_INET, &cliaddr.sin_addr, ip, INET_ADDRSTRLEN);
                    strncpy (user_data [i].ip, ip, sizeof (user_data [i].ip));
                    for (int j = 0; j < MAX_USER; j++)
                        user_data [i].banned [j] = 0;

                    user_data [i].port = ntohs (cliaddr.sin_port);
                    break;
                }
            }

            if (i == MAX_USER)
            {
                printf ("Too many users!\n");
                exit (-1);
            }

            hello (i);

            FD_SET (conn_fd, &allset);

            if (conn_fd > maxfd)
                maxfd = conn_fd;

            if (i > maxi)
                maxi = i;

            if (--ready <= 0)
                continue;
        }

        for (int i = 0; i <= maxi; i++)
        {
            if ((sock_fd = user_data [i].fd) < 0)
                continue;
            if (FD_ISSET (sock_fd, &rset))
            {
                if (readline (sock_fd, recv, MAX_STRING) == 0)
                {
                    close (sock_fd);
                    FD_CLR (sock_fd, &allset);
                    user_data [i].fd = -1;
                    offline (i);
                }
                else
                    read_cmd (i, recv);

                if (--ready <= 0)
                    break;
            }
        }
    }
}

int main (int argc, char **argv)
{
    if (argc != 2)
    {
        printf ("[Usage] ./server [server port]\n");
        exit (-1);
    }

    for (int i = 0; i < MAX_USER; i++)
        user_data [i].fd = -1;

    int listen_fd;
    socklen_t len;
    struct sockaddr_in servaddr;

    listen_fd = socket (AF_INET, SOCK_STREAM, 0);

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons (atoi (argv [1]));
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);

    bind (listen_fd, (struct sockaddr *) &servaddr, sizeof (servaddr));

    listen (listen_fd, MAX_USER);

    signal (SIGINT, sig_handler);
    server (listen_fd);
}
