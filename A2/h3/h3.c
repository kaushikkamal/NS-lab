#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_LINE 10
#define LINSTENPORT 7788
#define SERVERPORT 8877
#define BUFFSIZE 10

void writefile(int sockfd, FILE *fp);
ssize_t total = 0;

int main(int argc, char *argv[])
{
    char buff[BUFFSIZE] = {0};
    struct timeval start_time;
    struct timeval end_time;

    // if (argc != 3)
    // {
    //     perror("usage:./client u <filepath>");
    //     exit(1);
    // }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Can't allocate sockfd");
        exit(1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    if (connect(sockfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Connect Error");
        exit(1);
    }

    // getting the user operation
    strcpy(buff, argv[1]);

    // strcpy(buff, "download");
    send(sockfd, buff, BUFFSIZE, 0);

    char *filename = basename(argv[2]);

    if (filename == NULL)
    {
        perror("Can't get filename");
        exit(1);
    }
    printf("filename: %s\n\n", filename);
    strncpy(buff, filename, strlen(filename));
    if (send(sockfd, buff, BUFFSIZE, 0) == -1)
    {
        perror("Can't send filename");
        exit(1);
    }

    printf("\nDownloading File... \"%s\" \n", filename);

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("Can't open file");
        exit(1);
    }

    gettimeofday(&start_time, NULL);
    writefile(sockfd, fp);
    gettimeofday(&end_time, NULL);

    printf("Successfully download, Total Bytes = %ld\n", total);
    printf("Time taken: %ld sec\n", end_time.tv_sec - start_time.tv_sec);
    printf("Download rate: %ld B/s (%.2lf MB/s)\n\n", total / (end_time.tv_sec - start_time.tv_sec), (double)(total / (end_time.tv_sec - start_time.tv_sec)) / 1000000);

    fclose(fp);
    close(sockfd);
    return 0;
}

// write file fn def
void writefile(int sockfd, FILE *fp)
{
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0)
    {
        total += n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }

        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
    }
}
