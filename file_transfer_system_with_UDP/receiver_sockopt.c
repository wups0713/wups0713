#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BUF_SIZE 1000
#define HEADER_SIZE 8

struct Data
{
    char *buf;
    int size;
    int ack;
};

void server (int sockfd, char *filename)
{
    int n, pack_size, pack_count = -1, min_id = -1;
    struct sockaddr cliaddr;
    socklen_t len = sizeof (cliaddr);
    char recvline [BUF_SIZE];
    struct Data *data;

    int fp = open (filename, O_WRONLY | O_CREAT, 0666);

    while (1)
    {
        // get data
        do
        {
            n = recvfrom (sockfd, recvline, BUF_SIZE, 0, &cliaddr, &len);
        } while (n < 0);
        recvline [n] = 0;

        // deal with header
        int id;
        sscanf (recvline, "%4d%4d", &id, &pack_size);

        if (pack_size > 0)
        {
            // put data to buffer
            if (!data [id].ack)
            {
                data [id].buf = malloc (sizeof (char) * BUF_SIZE);
                for (int i = 0; i < pack_size; i++)
                    data [id].buf [i] = recvline [i + HEADER_SIZE];
            }
            data [id].size = pack_size;
            data [id].ack = 1;

            printf ("get %d\n", id);

            // send ack
            sendto (sockfd, recvline, 4, 0, &cliaddr, len);
        }
        else
        {
            // make buffer
            if (pack_count == -1)
            {
                pack_count = id;
                data = malloc (id * sizeof (struct Data));
                for (int i = 0; i < id; i++)
                {
                    data [i].buf = 0;
                    data [i].size = 0;
                    data [i].ack = 0;
                }
            }

            // send ack
            sendto (sockfd, "-1", 2, 0, &cliaddr, len);
        }

        // move sliding window
        while (data [min_id + 1].ack)
        {
            min_id++;
            do
            {
                n = write (fp, data [min_id].buf, data [min_id].size);
            } while (n < 0);
            free (data [min_id].buf);
        }

        printf ("%d / %d\n", min_id + 1, pack_count);

        //stop func *****
        if (min_id == pack_count - 1)
            break;
    }

    close (fp);
}

int main (int argc, char **argv)
{
    if (argc != 3)
    {
        printf ("Usage: ./%s [save filename] [bind port]\n", argv [0]);
        exit (-1);
    }

    int sockfd; 
    sockfd = socket (AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf ("Fail to open socket.\n");
        exit (-1);
    }

    struct sockaddr_in servaddr;
    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons (atoi (argv [2]));

    bind (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));

    server (sockfd, argv [1]);
}