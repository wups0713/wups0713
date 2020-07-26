#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BUF_SIZE 1000
#define HEADER_SIZE 8
#define MAXLEN 960

int timeo;

struct Data
{
    char *buf;
    int size;
    int ack;
};

void sigalrm_handler (int sig)
{
    timeo = 1;
    return;
}

void my_send (int sockfd, struct Data *data, int id, struct sockaddr *servaddr, socklen_t socklen)
{
    // size of sending data
    int filesize = 0;
    if (data)
        filesize = data [id].size;

    // send data
    char str [BUF_SIZE];
    sprintf (str, "%.4d%.4d", id, filesize);

    if (data)
        for (int i = 0; i < filesize; i++)
            str [i + HEADER_SIZE] = data [id].buf [i];

    sendto (sockfd, str, HEADER_SIZE + filesize, 0, servaddr, socklen);
}

void client (struct sockaddr *servaddr, socklen_t socklen, char *filename)
{
    int window_size = 32;

    // set clock
    struct itimerval two_RTT = {{0, 0}, {0, 500000}};

    // set signal
    struct sigaction act;
    act.sa_handler = sigalrm_handler;
    act.sa_flags = 0;
    sigaction (SIGALRM, &act, 0);

    // set socket
    int sockfd;
    sockfd = socket (AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        //printf ("Fail to open socket.\n");
        exit (-1);
    }

    // set up
    struct stat st;
    stat (filename, &st);
    int pack_count = st.st_size / MAXLEN + ((st.st_size % MAXLEN)?1:0);

    // get RTT
    int fail = 0;
    while (1)
    {
        char recvline [10];
        //struct timeval start, end;

        //printf ("test RTT\n");

        my_send (sockfd, 0, pack_count, servaddr, socklen);

        setitimer (ITIMER_REAL, &two_RTT, NULL);

        //gettimeofday (&start, NULL);
        if (recvfrom (sockfd, recvline, 10, 0, NULL, NULL) > 0)
        {
            //gettimeofday (&end, NULL);
            //two_RTT.it_value.tv_sec = 2 * (end.tv_sec - start.tv_sec);
            //two_RTT.it_value.tv_usec = 2 * (end.tv_usec - start.tv_usec);
            printf ("get RTT %ld sec, %ld usec\n", two_RTT.it_value.tv_sec, two_RTT.it_value.tv_usec);
            break;
        }
        else
        {
            printf ("fail\n");
            fail++;
            if (fail % 5 == 0 && two_RTT.it_value.tv_sec < 5)
            {
                if (two_RTT.it_value.tv_usec == 500000)
                {
                    two_RTT.it_value.tv_usec = 0;
                    two_RTT.it_value.tv_sec++;
                }
                else
                    two_RTT.it_value.tv_usec = 500000;
            }
        }
    }

    // make buffer
    struct Data data [pack_count];
    for (int i = 0; i < pack_count; i++)
    {
        data [i].buf = 0;
        data [i].ack = 0;
    }

    // start to send
    int min_id = 0, sended, acked, end = 0;
    int fp = open (filename, O_RDONLY);

    while (1)
    {
        sended = 0;
        acked = 0;

        for (int i = 0; i < window_size && min_id + i < pack_count; i++)
        {
            int id = min_id + i;

            if (data [id].ack)
                continue;

            // read file
            if (data [id].buf == NULL)
            {
                data [id].buf = malloc (sizeof (char) * BUF_SIZE);

                int n;
                do
                {
                    n = read (fp, data [id].buf, MAXLEN);
                } while (n < 0);

                data [id].size = n;
            }

            // send data
            //printf ("send %d with size %d\n", id, data [id].size);
            my_send (sockfd, data, id, servaddr, socklen);
            sended++;
        }

        // set timer
        timeo = 0;
        setitimer (ITIMER_REAL, &two_RTT, NULL);

        // get ACK
        int ack, id;
        char recvline [10];

        while (!timeo && sended != acked)
        {
            if ((ack = recvfrom (sockfd, recvline, 10, 0, NULL, NULL)) < 0)
            {
                if (errno == EINTR)
                {
                    //printf ("timeout\n");
                    break;
                }
                //else
                    //printf ("error\n");
            }
            else
            {
                sscanf (recvline, "%4d", &id);
                if (id >= min_id && id < min_id + window_size)
                {
                    acked++;
                    //printf ("get %d ack\n", id);
                    data [id].ack = 1;
                }
            }
        }

        // move sliding window
        while (min_id < pack_count && data [min_id].ack)
        {
            free (data [min_id].buf);
            min_id++;
        }

        printf ("%d %d / %d\n", window_size, acked, sended);

        // change sliding window size
        if (2 * sended > window_size)
            if (3 * sended < 4 * acked)
                window_size++;
            else 
                window_size = window_size / 2;

        if (window_size < 8)
            window_size = 8;

        // stop
        if (min_id == pack_count)
            break;

        if (acked == 0)
            end++;
        else
            end = 0;

        if (end == 10)
            break;        
    }

    close (sockfd);
    close (fp);
}

int main (int argc, char **argv)
{
    if (argc != 4)
    {
        printf ("Usage: ./%s [send filename] [target address] [connect port]\n", argv [0]);
        exit (-1);
    }

    struct sockaddr_in servaddr;
    struct hostent *he;

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = inet_addr (argv [2]);
    servaddr.sin_port = htons (atoi (argv [3]));

    if (inet_pton (AF_INET, argv [2], &servaddr.sin_addr) != 1)
    {
        if ((he = gethostbyname(argv[2])) == NULL)
        {
            printf ("address error\n");
            exit (-1);
        }
        else
            memcpy (&servaddr.sin_addr, he->h_addr_list[0], sizeof (struct in_addr));
    }

    client ((struct sockaddr *) &servaddr, sizeof (servaddr), argv [1]);
}