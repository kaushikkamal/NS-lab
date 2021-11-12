#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

int main()
{
    int sockfd = 0, n = 0, num = 0, apple, mango;
    char appleAmount[50], mangoAmount[50], receiverBuffer[1024], senderBuffer[1025];
    struct sockaddr_in serverAddress, clientAddress;

    memset(receiverBuffer, '0', sizeof(receiverBuffer));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5000);
    serverAddress.sin_addr.s_addr = inet_addr("10.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("\nError : Connect Failed\n");
        return 1;
    }

    recv(sockfd, appleAmount, sizeof(appleAmount), 0);
    apple = atoi(appleAmount);

    recv(sockfd, mangoAmount, sizeof(mangoAmount), 0);
    mango = atoi(mangoAmount);

    printf("\nWelcome to our shop\n");
    printf("Available items\n");
    printf("\nProduct\tQuantity");
    printf("\nApple\t%d\nMango\t%d\n", apple, mango);
    printf("\nPlease enter 'buy fruitname 10' where 10 is the quantity\n");
    fgets(senderBuffer, 1025, stdin);

    if ((send(sockfd, senderBuffer, strlen(senderBuffer), 0)) == -1)
    {
        fprintf(stderr, "Failure Sending Message\n");
        close(sockfd);
    }

    num = recv(sockfd, receiverBuffer, sizeof(receiverBuffer), 0);
    if (num <= 0)
    {
        printf("Either Connection Closed or Error\n");
    }

    receiverBuffer[num] = '\0';
    printf("\nFrom Server: %s\n\n", receiverBuffer);

    close(sockfd);
    return 0;
}