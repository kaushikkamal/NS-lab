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
#include <unistd.h>

#define SENTENCE 10

int main()
{
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;

    FILE *file, *wFile;
    int i, j;
    char arr[SENTENCE][100];
    char key = 'X';
    int fd;
    struct stat file_stat;
    socklen_t sock_len;
    char file_size[256];
    off_t offset;
    int remain_data;
    ssize_t len;
    int sent_bytes = 0;

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    file = fopen("sen.txt", "r");
    wFile = fopen("encrypted.txt", "w");

    if (file == NULL || wFile == NULL)
    {
        return -1;
    }

    for (i = 0; i < SENTENCE; i++)
    {
        fgets(arr[i], sizeof(arr[i]), file);
    }

    printf("Encoded message: \n");

    for (i = 0; i < SENTENCE; i++)
    {
        for (j = 0; j < strlen(arr[i]); j++)
        {
            arr[i][j] = arr[i][j] ^ key;
            printf("%c", arr[i][j]);
        }
        fputs(arr[i], wFile);
        fputs("\n", wFile);
        printf("\n");
    }
    fclose(wFile);

    fd = open("encrypted.txt", O_RDONLY);
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

    len = send(clientSocket, file_size, sizeof(file_size), 0);
    if (len < 0)
    {
        fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }
    offset = 0;
    remain_data = file_stat.st_size;

    while (((sent_bytes = sendfile(clientSocket, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0))
    {
        // fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %lu and remaining data = %d\n", sent_bytes, (long unsigned)offset, remain_data);
        remain_data -= sent_bytes;
        // fprintf(stdout, "2. Server zsent %d bytes from file's data, offset is now : %lu and remaining data = %d\n", sent_bytes, (long unsigned)offset, remain_data);
    }

    close(clientSocket);

    // printf("\n");
    // printf("decoded message: ");
    // for (i = 0; i < SENTENCE; i++)
    // {
    //     for (j = 0; j < strlen(arr[i]); j++)
    //     {
    //         arr[i][j] = arr[i][j] ^ key;
    //         printf("%c", arr[i][j]);
    //     }
    //     printf("\n");
    // }

    // strcpy(buffer, "Hi\n");
    // send(clientSocket, buffer, 1024, 0);

    // recv(clientSocket, buffer, 1024, 0);
    // printf("\nData received from server: %s\n", buffer);

    return 0;
}

