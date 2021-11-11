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
    char id[100];
    char sendBuff[1025];
    char recvBuff[1024];
    int numrv;
    int apple = 20, mango = 10, sav;
    char appleAmount[50], mangoAmount[50];
    char new1[50];
    int new, i, j = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.0.0.1");
    serv_addr.sin_port = htons(5000);

    memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);
    bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    while (1)
    {
        j = 1;

        addr_size = sizeof serverStorage;
        connfd = accept(listenfd, (struct sockaddr *)&serverStorage, &addr_size);

        struct sockaddr_in *cliIP = (struct sockaddr_in *)&serverStorage;
        struct in_addr ipAddr = cliIP->sin_addr;

        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

        snprintf(appleAmount, sizeof(appleAmount), "%d", apple);
        send(connfd, appleAmount, strlen(appleAmount), 0);

        sleep(1);

        snprintf(mangoAmount, sizeof(mangoAmount), "%d", mango);
        send(connfd, mangoAmount, strlen(mangoAmount), 0);

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

        printf("\nClient IP is: %s", inet_ntoa(serverStorage.sin_addr));
        printf("\nClient port is: %d", serverStorage.sin_port);
        printf("\nMessage received From client : %s", recvBuff);

        if (recvBuff[4] == 'a')
        {
            sav = apple;
            apple = apple - new;
            if (apple < 0)
            {
                apple = sav;
                printf("Sorry, the given quantity of Apples is not available");
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
                printf("Sorry, the given quantity of Mangos is not available");
            }
            else
            {
                printf("\n%d Mangos Sold\n", new);
                transactionID++;
            }
        }

        snprintf(id, sizeof(id), "%d", transactionID);
        memset(sendBuff, '0', sizeof(sendBuff));
        strcpy(sendBuff, "Transaction ID: ");
        strcat(sendBuff, id);

        if ((send(connfd, sendBuff, strlen(sendBuff), 0)) == -1)
        {
            fprintf(stderr, "Failure Sending Message\n");
            close(connfd);
            break;
        }

        sleep(1);
    } //End of Inner While...

    close(connfd); //Close Connection Socket
    return 0;
}