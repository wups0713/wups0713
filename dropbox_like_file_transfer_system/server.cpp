#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_LEN 1500

struct Client
{
    int fd;
    int user;
    int file_count;
    char *file_name;
    int file_size;
    std::vector <bool> files;
    int state;
    int download_fd;
    bool tried;
    int upload_fd;
    int file_now;
    int file_max;
};

struct User
{
    char username [30];
    int file_count;
    std::vector <char *> files;
    std::vector <int> file_size;
};

std::vector <User> users;
std::vector <Client> clients;
int user_count = 0;
int client_count = 0;

void read_cmd (int id, char *recv_msg, int n)
{
    int user = clients [id].user;
    int sock = clients [id].fd;

    if (clients [id].user == -1)
    {
        for (int i = 0; i < user_count; i++)
            if (!strcmp (users [i].username, recv_msg))
            {
                clients [id].user = i;
                clients [id].files = std::vector <bool> (users [i].file_count, false);
                clients [id].state = 0;
                return;
            }

        User new_user;
        strcpy (new_user.username, recv_msg);
        new_user.file_count = 0;
        
        clients [id].user = user_count;
        clients [id].state = 0;

        users.push_back (new_user);
        user_count++;

        char path [40] = "./";
        strcat (path, recv_msg);
        struct stat st;
        if (stat (path, &st) == -1)
            mkdir (path, 0777);
        return;
    }

    if (clients [id].state == 0 && recv_msg [0] == '1')
    {
        clients [id].state = 1;
        char path [70];
        clients [id].file_name = new char [30];
        sscanf (recv_msg + 1, "%d%s", &clients [id].file_size, clients [id].file_name);
        sprintf (path, "./%s/%s", users [user].username, clients [id].file_name);
        clients [id].file_now = 0;
        clients [id].file_max = clients [id].file_size;
        clients [id].upload_fd = open (path, O_WRONLY | O_CREAT, 0666);

        send (sock, "1", 1, 0);
        usleep (10000);
        return;
    }

    if (clients [id].state == 1)
    {
        if (n + clients [id].file_now > clients [id].file_max)
            n = clients [id].file_max - clients [id].file_now;
        write (clients [id].upload_fd, recv_msg, n);
        clients [id].file_now += n;

        if (clients [id].file_now == clients [id].file_max)
        {
            close (clients [id].upload_fd);
            users [user].file_count++;
            users [user].files.push_back (clients [id].file_name);
            users [user].file_size.push_back (clients [id].file_size);

            for (int i = 0; i < client_count; i++)
            {
                int tmp = clients [i].user;
                if (tmp == user)
                {
                    if (i == id)
                    {
                        clients [i].file_count++;
                        clients [i].files.push_back (true);
                    }
                    else
                        clients [i].files.push_back (false);
                }
            }

            clients [id].state = 0;
            clients [id].tried = false;
            clients [id].file_now = 0;
        }
        return;
    }

    if (recv_msg [0] == '2')
    {
        if (clients [id].state == 0)
            clients [id].state = 2;
        else
            clients [id].state = 0;

        clients [id].tried = false;
        return;
    }

    if (recv_msg [0] == '3')
    {
        if (clients [id].state == 0)
            clients [id].state = 3;
        else
            clients [id].state = 0;
        return;
    }
}

void server (int listen_fd)
{
    int maxfdp1 = listen_fd + 1, sock_fd, conn_fd, ready, n;
    char send_msg [MAX_LEN], recv_msg [MAX_LEN];
    sockaddr_in cliaddr;
    fd_set allset, rset;
    FD_ZERO (&allset);
    FD_SET (listen_fd, &allset);
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 0;

    while (1)
    {
        rset = allset;
        ready = select (maxfdp1, &rset, NULL, NULL, &t);

        if (FD_ISSET (listen_fd, &rset))
        {
            socklen_t clilen = sizeof (cliaddr);
            
            conn_fd = accept (listen_fd, (struct sockaddr *) &cliaddr, &clilen);
            int flag = fcntl (conn_fd, F_GETFL, 0);
            fcntl (conn_fd, F_SETFL, flag | O_NONBLOCK);

            Client new_client;
            new_client.fd = conn_fd;
            new_client.user = -1;
            new_client.state = -1;
            new_client.file_count = 0;
            new_client.download_fd = -1;
            new_client.tried = false;

            clients.push_back (new_client);
            client_count++;

            FD_SET (conn_fd, &allset);

            if (conn_fd >= maxfdp1)
                maxfdp1 = conn_fd + 1;

            if (--ready <= 0)
                continue;
        }

        for (int i = 0; i < client_count; i++)
        {
            int user = clients [i].user;
            sock_fd = clients [i].fd;

            if (FD_ISSET (sock_fd, &rset))
            {
                if ((n = recv (sock_fd, recv_msg, MAX_LEN, 0)) == 0)
                {
                    close (sock_fd);
                    FD_CLR (sock_fd, &allset);
                    clients.erase (clients.begin () + i);
                    i--;
                    client_count--;
                }
                else
                {
                    recv_msg [n] = '\0';
                    read_cmd (i, recv_msg, n);
                    continue;
                }
                
                if (--ready <= 0)
                    break;
            }

            if (clients [i].state == 0 && !clients [i].tried && clients [i].file_count < users [user].file_count)
            {
                int file_num;
                for (int j = 0; j < users [user].file_count; j++)
                    if (!clients [i].files [j])
                    {
                        file_num = j;
                        break;
                    }

                n = sprintf (send_msg, "2%d %s", users [user].file_size [file_num], users [user].files [file_num]);
                if (clients [i].download_fd == -1)
                {
                    char path [70];
                    sprintf (path, "./%s/%s", users [user].username, users [user].files [file_num]);
                    clients [i].download_fd = open (path, O_RDONLY);
                }

                send (sock_fd, send_msg, n, 0);
                usleep (10000);

                clients [i].tried = true;
                clients [i].file_max = users [user].file_size [file_num];
                clients [i].file_now = 0;
            }

            if (clients [i].state == 2 && clients [i].download_fd != -1)
            {
                n = read (clients [i].download_fd, send_msg, 1000);
                int send_n = send (sock_fd, send_msg, n, 0);

                if (send_n > 0)
                {
                    clients [i].file_now += send_n;

                    if (send_n < n)
                        lseek (clients [i].download_fd, send_n - n, SEEK_CUR);
                    
                    if (clients [i].file_now == clients [i].file_max)
                    {
                        close (clients [i].download_fd);
                        usleep (10000);
                        clients [i].download_fd = -1;
                        clients [i].file_now = 0;
                        for (int j = 0;;j++)
                            if (!clients [i].files [j])
                            {
                                clients [i].files [j] = true;
                                break;
                            }
                        clients [i].file_count++;
                    }
                }
                else
                    lseek (clients [i].download_fd, -n, SEEK_CUR);
            }
        }
    }
}

int main (int argc, char **argv)
{
    if (argc != 2)
        exit (-1);

    int listen_fd;
    sockaddr_in servaddr;

    listen_fd = socket (AF_INET, SOCK_STREAM, 0);
    int flag = fcntl(listen_fd, F_GETFL, 0);
    fcntl (listen_fd, F_SETFL, flag | O_NONBLOCK);

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons (atoi (argv [1]));
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);

    bind (listen_fd, (sockaddr *) &servaddr, sizeof (servaddr));

    listen (listen_fd, 20);

    server (listen_fd);
}