#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

int main()
{
    struct sockaddr_in serverAddress;
    struct sockaddr_in serverStorage;
    socklen_t addressSize;

    int listenfd = 0, connfd = 0, n = 0, num = 0, transactionID = 0, apple = 20, mango = 10, temp, new, i, j, flag, error;
    char id[100], senderBuffer[1025], receiverBuffer[1024], appleAmount[50], mangoAmount[50], new1[50];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serverAddress, '0', sizeof(serverAddress));
    memset(senderBuffer, '0', sizeof(senderBuffer));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("10.0.0.1");
    serverAddress.sin_port = htons(5000);

    memset(serverAddress.sin_zero, '\0', sizeof serverAddress.sin_zero);
    bind(listenfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    while (1)
    {
        j = 1;
        flag = 0;
        error = 0;

        // printf("\nAvailable items\n");
        // printf("\nProduct\tQuantity");
        // printf("\nApple\t%d\nMango\t%d\n", apple, mango);

        printf("\nWaiting for order...\n");

        addressSize = sizeof serverStorage;
        connfd = accept(listenfd, (struct sockaddr *)&serverStorage, &addressSize);

        struct sockaddr_in *cliIP = (struct sockaddr_in *)&serverStorage;
        struct in_addr ipAddr = cliIP->sin_addr;

        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

        snprintf(appleAmount, sizeof(appleAmount), "%d", apple);
        send(connfd, appleAmount, strlen(appleAmount), 0);

        sleep(1);

        snprintf(mangoAmount, sizeof(mangoAmount), "%d", mango);
        send(connfd, mangoAmount, strlen(mangoAmount), 0);

        num = recv(connfd, receiverBuffer, sizeof(receiverBuffer), 0);
        if (num <= 0)
        {
            printf("Either Connection Closed or Error\n");
        }

        receiverBuffer[num] = '\0';
        for (i = 10; i < strlen(receiverBuffer); i++)
        {
            new1[j - 1] = receiverBuffer[i];
            j++;
        }
        new1[j - 1] = '\0';
        new = atoi(new1);

        printf("\nClient IP is: %s", inet_ntoa(serverStorage.sin_addr));
        printf("\nClient port is: %d", serverStorage.sin_port);
        printf("\nOrder Received From client : %s", receiverBuffer);

        if (receiverBuffer[4] == 'a')
        {
            temp = apple;
            apple = apple - new;
            if (apple < 0)
            {
                apple = temp;
                strcpy(senderBuffer, "Quantity not available");
                // printf("Quantity not available");
                error = 1;
            }
            else
            {
                printf("\n%d Apples Sold\n", new);
                transactionID++;
                flag = 1;
            }
        }

        if (receiverBuffer[4] == 'm')
        {
            temp = mango;
            mango = mango - new;
            if (mango < 0)
            {
                mango = temp;
                strcpy(senderBuffer, "Quantity not available");
                // printf("Quantity not available");
                error = 1;
            }
            else
            {
                printf("\n%d Mangoes Sold\n", new);
                transactionID++;
                flag = 1;
            }
        }

        if (error == 0)
        {
            snprintf(id, sizeof(id), "%d", transactionID);
            memset(senderBuffer, '0', sizeof(senderBuffer));
            strcpy(senderBuffer, "Transaction ID: ");
            strcat(senderBuffer, id);
        }

        if ((send(connfd, senderBuffer, strlen(senderBuffer), 0)) == -1)
        {
            fprintf(stderr, "Failure Sending Message\n");
            close(connfd);
            break;
        }

        printf("\n-----------------------------------\n");
        if (flag)
        {
            printf("\tTransaction %d Ends", transactionID);
            printf("\n-----------------------------------\n");
        }

        sleep(1);
    }

    close(connfd);
    return 0;
}
