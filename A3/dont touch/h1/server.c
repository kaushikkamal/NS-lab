#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/types.h>

#define EN_FILENAME "encrypted.txt"
#define DE_FILENAME "decrypted.txt"
#define SENTENCE 10

int main()
{
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in serverStorage;
    socklen_t addr_size;

    char key;
    int i, j;
    ssize_t len;
    struct sockaddr_in remote_addr;
    char buffer[BUFSIZ];
    char arr[SENTENCE][100];
    int file_size;
    FILE *file, *received_file;
    int remain_data = 0;
    double time_taken;

    key = 'X';

    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (listen(serverSocket, 5) == 0)
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

        newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);

        recv(newSocket, buffer, BUFSIZ, 0);
        file_size = atoi(buffer);

        received_file = fopen(EN_FILENAME, "w");
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

        fclose(received_file);

        close(newSocket);

        received_file = fopen(EN_FILENAME, "r");
        file = fopen(DE_FILENAME, "w");
        if (received_file == NULL)
        {
            return -1;
        }

        // for (i = 0; i < SENTENCE; i++)
        // {
        //     fgets(arr[i], sizeof(arr[i]), received_file);
        // }
        // printf("Encoded message: ");

        // for (i = 0; i < SENTENCE; i++)
        // {
        //     printf("%s", arr[i]);
        //     printf("\n");
        // }

        // for (i = 0; i < SENTENCE; i++)
        // {
        //     for (j = 0; j < strlen(arr[i]); j++)
        //     {
        //         arr[i][j] = arr[i][j] ^ key;
        //         // printf("%c", arr[i][j]);
        //     }
        //     fputs(arr[i], file);
        //     fputs("\n", file);
        //     printf("\n");
        // }

        fclose(received_file);
        fclose(file);
    }
    close(serverSocket);
    return 0;
}

