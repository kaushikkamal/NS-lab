#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <time.h>

#define FILE_TO_SEND "1.pdf"

int main()
{
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;

    int peer_socket;
    socklen_t sock_len;
    ssize_t len;
    struct sockaddr_in peer_addr;
    int fd;
    int sent_bytes = 0;
    char file_size[256];
    struct stat file_stat;
    off_t offset;
    int remain_data;
    double time_taken;
    clock_t t;

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    fd = open(FILE_TO_SEND, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Error opening file --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &file_stat) < 0)
    {
        fprintf(stderr, "Error fstat --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "File Size: \n%lu bytes\n", (long unsigned)file_stat.st_size);

    sock_len = sizeof(struct sockaddr_in);

    sprintf(file_size, "%lu", (long unsigned)file_stat.st_size);

    t = clock();

    len = send(clientSocket, file_size, sizeof(file_size), 0);

    if (len < 0)
    {
        fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server sent %lu bytes for the size\n", (long unsigned)len);

    offset = 0;
    remain_data = file_stat.st_size;

    while (((sent_bytes = sendfile(clientSocket, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0))
    {
        // fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %lu and remaining data = %d\n", sent_bytes, (long unsigned)offset, remain_data);
        remain_data -= sent_bytes;
        // fprintf(stdout, "2. Server zsent %d bytes from file's data, offset is now : %lu and remaining data = %d\n", sent_bytes, (long unsigned)offset, remain_data);
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

    printf("\ntook %f seconds to execute.\n", time_taken);
    close(peer_socket);
    close(clientSocket);
    return 0;
}

