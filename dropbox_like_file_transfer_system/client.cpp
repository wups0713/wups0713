#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_LEN 1500

int pid;

void cmd (int sock_fd, char *msg)
{
    int upload, n, send_n;
    char recv_msg [MAX_LEN], send_msg [MAX_LEN];
    char *cmd = strtok (msg, " ");
    char *arg = strtok (NULL, "\n");
    if (arg == NULL)
        return;

    if (!strcmp (cmd, "put"))
    {
        upload = open (arg, O_RDONLY);
        if (upload < 0)
            return;

        struct stat st;
        if (stat (arg, &st) == -1)
            return;

        n = sprintf (send_msg, "1%d %s", (int)st.st_size, arg);
        send (sock_fd, send_msg, n, 0);
        usleep (100000);

        while (n = recv (sock_fd, recv_msg, MAX_LEN, 0))
        {
           recv_msg [n] = '\0';
           if (recv_msg [n - 1] == '1')
               break;
        }

        int total_send = 0;

        std::cout << "Pid : " << pid << " [Upload] " << arg << " Start!\n";
        while (total_send < st.st_size)
        {
            n = read (upload, send_msg, 1000);
            send_n = send (sock_fd, send_msg, n, 0);

            if (send_n > 0)
            {
                total_send += send_n;

                std::cout << "\rPid : " << pid << " Progess : [";
                for (int i = 0; i < 20; i++)
                    if (total_send * 20 > i * st.st_size)
                        std::cout << "#";
                    else
                        std::cout << " ";
                std::cout << "]";

                if (send_n < n)
                    lseek (upload, send_n - n, SEEK_CUR);
            }
            else
                lseek (upload, -n, SEEK_CUR);
        }

        std::cout << "\nPid : " << pid << " [Upload] " << arg << " Finish!\n";
        close (upload);
        return;
    }

    if (!strcmp (cmd, "sleep"))
    {
        send (sock_fd, "3", 1, 0);
        std::cout << "Pid : " << pid << " The client starts to sleep.\n";
        int sec = atoi (arg);

        for (int i = 0; i < sec; )
        {
            sleep (1);
            std::cout << "Pid : " << pid << " Sleep " << ++i << std::endl;
            if (i == 19)
                send (sock_fd, "3", 1, 0);
        }

        std::cout << "Pid : " << pid << " Client wakes up.\n";
        return;
    }
}

void client (int sock_fd, char *name)
{
    int maxfdp1 = sock_fd + 1, end = 0, n, start = 0;
    fd_set rset;
    FD_ZERO (&rset);
    char send_msg [MAX_LEN], recv_msg [MAX_LEN];

    send (sock_fd, name, strlen (name), 0);
    std::cout << "Pid : " << pid << " Welcome to the dropbox-like server: " << name << std::endl;

    while (1)
    {
        if (!end)
            FD_SET (0, &rset);
        FD_SET (sock_fd, &rset);

        select (maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET (0, &rset))
        {
            if (fgets (send_msg, MAX_LEN, stdin) == 0 || !strcmp (send_msg, "exit\n"))
            {
                end = 1;
                shutdown (sock_fd, SHUT_WR);
                continue;
            }

            cmd (sock_fd, send_msg);
        }

        if (FD_ISSET (sock_fd, &rset))
        {
            if ((n = recv (sock_fd, recv_msg, MAX_LEN, 0)) == 0)
            {
                if (!end)
                    exit (-1);
                return;
            }

            recv_msg [n] = '\0';

            if (recv_msg [0] == '2')
            {
                int total_recv = 0, recv_n, download, filesize;
                char filename [30];
                sscanf (recv_msg + 1, "%d%s", &filesize, filename);
                download = open (filename, O_WRONLY | O_CREAT, 0666);

                send (sock_fd, "2", 1, 0);

                std::cout << "Pid : " << pid << " [Download] " << filename << " Start!\n";
                while (total_recv < filesize)
                {
                    recv_n = recv (sock_fd, recv_msg, MAX_LEN, 0);

                    if (recv_n > 0)
                    {
                        if (recv_n + total_recv > filesize)
                            recv_n = filesize - total_recv;
                        write (download, recv_msg, recv_n);
                        total_recv += recv_n;

                        std::cout << "\rPid : " << pid << " Progess : [";
                        for (int i = 0; i < 20; i++)
                            if (total_recv * 20 > i * filesize)
                                std::cout << "#";
                            else
                                std::cout << " ";
                        std::cout << "]";
                    }
                }

                std::cout << "\nPid : " << pid << " [Download] " << filename << " Finish!\n";
                send (sock_fd, "2", 1, 0);
                usleep (100000);

                close (download);
            }
        }
    }
}

int main (int argc, char **argv)
{
    pid = getpid ();

    if (argc != 4)
        exit (-1);

    int sock_fd;
    struct sockaddr_in servaddr;

    sock_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        exit (-1);

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons (atoi (argv [2]));
    inet_pton (AF_INET, argv [1], &(servaddr.sin_addr));

    connect (sock_fd, (struct sockaddr *) &servaddr, sizeof (servaddr));

    client (sock_fd, argv [3]);
}