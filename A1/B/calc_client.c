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
  int sockfd = 0,n = 0,num=0;
  int op1, op2, res;
  char recvBuff[1024];
  char sendBuff[1025];
  struct sockaddr_in serv_addr;
  memset(recvBuff, '0' ,sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
      printf("\n Error : Could not create socket \n");
      return 1;
    }
 
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000);//assign the port number
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//assign a the ip address of server
 
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
      printf("\n Error : Connect Failed \n");
      return 1;
    }
	//after connection write the data to server
    printf("Welcome to Calculator\n");
	printf("Please enter the expression as calc a*b\n");
    fgets(sendBuff,1025,stdin);
                if ((send(sockfd,sendBuff,strlen(sendBuff),0))== -1) 
                {
                     fprintf(stderr, "Failure Sending Message\n");
                     close(sockfd);
                }

                num = recv(sockfd, recvBuff, sizeof(recvBuff),0);
                if ( num <= 0 )
                {
                        printf("Either Connection Closed or Error\n");
                       
                }

                recvBuff[num] = '\0';
                read(sockfd,&result,sizeof(res));
 				printf("Operation result from server=%d\n",res);
 				

    			close(sockfd); 
  return 0;
}
