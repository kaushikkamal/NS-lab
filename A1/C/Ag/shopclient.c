#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(void)
{
    int sockfd = 0, n = 0, num = 0, apple, mango;
    char appleAmount[50], mangoAmount[50];
    char recvBuff[1024];
    char sendBuff[1025];
    struct sockaddr_in serv_addr, clientAddr;
    memset(recvBuff, '0', sizeof(recvBuff));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("10.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nError : Connect Failed\n");
        return 1;
    }

    // read(sockfd, appleAmount, 50);

    recv(sockfd, appleAmount, sizeof(appleAmount), 0);

    apple = atoi(appleAmount);

    // read(sockfd, mangoAmount, 50);

    recv(sockfd, mangoAmount, sizeof(mangoAmount), 0);

    mango = atoi(mangoAmount);

    //after connection write the data to server
    printf("\nWelcome to our shop\n");
    printf("Available items\n");
    printf("\nProduct\tQuantity");
    printf("\nApple\t%d\nMango\t%d\n", apple, mango);
    printf("Please enter 'buy fruitname 10' where 10 is the quantity\n");
    fgets(sendBuff, 1025, stdin);

    if ((send(sockfd, sendBuff, strlen(sendBuff), 0)) == -1)
    {
        fprintf(stderr, "Failure Sending Message\n");
        close(sockfd);
    }

    num = recv(sockfd, recvBuff, sizeof(recvBuff), 0);
    if (num <= 0)
    {
        printf("Either Connection Closed or Error\n");
    }

    recvBuff[num] = '\0';
    printf("From Server: %s\n", recvBuff);

    close(sockfd);
    return 0;
}
