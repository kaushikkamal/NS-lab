#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>

int main()
{
    struct sockaddr_in serv_addr;
    struct sockaddr_in serverStorage;
    socklen_t addr_size;
    int listenfd = 0, connfd = 0, n = 0, num = 0;
    int transactionID = 0;
    char sendBuff[1025];
    char recvBuff[1024];
    int numrv;
    int mango = 10, apple = 20, sav;
    char new1[50];
    int new, i, j = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.0.0.1");
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    while (1)
    {
        j = 1;
        printf("\nAvailable items\n");
        printf("\nProduct\tQuantity");
        printf("\nApple\t%d\nMango\t%d\n", apple, mango);

        printf("\nWaiting for order...\n");

        addr_size = sizeof serverStorage;
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);

        struct sockaddr_in *cliIP = (struct sockaddr_in *)&serverStorage;
        struct in_addr ipAddr = cliIP->sin_addr;

        char str[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

        num = recv(connfd, recvBuff, sizeof(recvBuff), 0);
        if (num <= 0)
        {
            printf("Either Connection Closed or Error\n");
        }

        recvBuff[num] = '\0';
        for (i = 10; i < strlen(recvBuff); i++)
        {
            new1[j - 1] = recvBuff[i];
            j++;
        }
        new1[j - 1] = '\0';
        new = atoi(new1);

        printf("\nClient IP is: %s", str);
        printf("\nClient port is: %d", serverStorage.sin_port);
        printf("\nOrder Received From client : %s", recvBuff);

        if (recvBuff[4] == 'a')
        {
            sav = apple;
            apple = apple - new;
            if (apple < 0)
            {
                apple = sav;
                printf("Sorry, the requested quantity of Apples is not available");
            }
            else
            {
                printf("\n%d Apples Sold\n", new);
                transactionID++;
            }
        }

        if (recvBuff[4] == 'm')
        {
            sav = mango;
            mango = mango - new;
            if (mango < 0)
            {
                mango = sav;
                printf("Sorry, the requested quantity of Mangoes is not available");
            }
            else
            {
                printf("\n%d Mangoes Sold\n", new);
                transactionID++;
            }
        }

        char id[100];
        snprintf(id, sizeof(id), "%d", transactionID);
        strcpy(sendBuff, "Transaction ID: ");
        strcat(sendBuff, id);

        if ((send(connfd, sendBuff, strlen(sendBuff), 0)) == -1)
        {
            fprintf(stderr, "Failure Sending Message\n");
            close(connfd);
            break;
        }

        printf("\n------------------------------\n");
        printf("\tTransaction %d Ends", transactionID);
        printf("\n------------------------------\n");

        sleep(1);
    } //End of Inner While...

    close(connfd); //Close Connection Socket
    return 0;
}
