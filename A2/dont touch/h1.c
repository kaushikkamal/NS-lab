#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>

#define FILENAME "1.pdf"

int main()
{
    int welcomeSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in serverStorage;
    socklen_t addr_size;

    ssize_t len;
    struct sockaddr_in remote_addr;
    char buffer[BUFSIZ];
    int file_size;
    FILE *received_file;
    int remain_data = 0;
    double time_taken;
    clock_t t;

    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    bind(welcomeSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (listen(welcomeSocket, 5) == 0)
    {
        printf("Listening\n");
    }
    else
    {
        printf("Error\n");
    }

    while (1)
    {
        addr_size = sizeof serverStorage;

        newSocket = accept(welcomeSocket, (struct sockaddr *)&serverStorage, &addr_size);

        // t = clock();

        recv(newSocket, buffer, BUFSIZ, 0);
        file_size = atoi(buffer);

        received_file = fopen(FILENAME, "w");
        if (received_file == NULL)
        {
            fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

            exit(EXIT_FAILURE);
        }

        remain_data = file_size;

        while ((remain_data > 0) && ((len = recv(newSocket, buffer, BUFSIZ, 0)) > 0))
        {
            fwrite(buffer, sizeof(char), len, received_file);
            remain_data -= len;
            fprintf(stdout, "Receive %lu bytes\n", (long unsigned)len);
        }
        // t = clock() - t;
        fclose(received_file);

        // time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

        // printf("fun() took %f seconds to execute \n", time_taken);

        close(newSocket);
    }
    close(welcomeSocket);

    return 0;
}
