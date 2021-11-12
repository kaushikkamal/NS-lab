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
int main(void)
{
  int listenfd = 0,connfd = 0,n=0,num=0; 
  struct sockaddr_in serv_addr;
 
  char sendBuff[1025];
  char recvBuff[1024];  
  int numrv;  
  int mango=10,apple=20 ,sav;
  int op1, op2, res;
  char new1[50];
  int new,i;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("socket retrieve success\n");
  
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(sendBuff, '0', sizeof(sendBuff));
      
  serv_addr.sin_family = AF_INET;    
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  serv_addr.sin_port = htons(5000);    
 
  bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
  
  if(listen(listenfd, 10) == -1)
  {
      printf("Failed to listen\n");
      return -1;
  }
   	  
  while(1)
    {
      
        connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);	  // accept awaiting request
	 	//read the data send by client
		num = recv(connfd, recvBuff, sizeof(recvBuff),0);
                if ( num <= 0 )
                {
                        printf("Either Connection Closed or Error\n");
                }

                recvBuff[num] = '\0';
                for(i=1;i<strlen(recvBuff);i++)
                 {
             		new1[i-1]=recvBuff[i];
                 }
                new1[i-1]='\0';
                new=atoi(new1);
             
                if(recvBuff[6]=='+')
                  {		
    
						op1 = recvBuff[5] - '0';
                  		op2 = recvBuff[7] - '0';
                  		//printf("Operator %d and %d and result %d", op1, op2, op1 + op2);
                  		res = op1 + op2;
                  	
                   }
			   
			   //write the data for client   
               //fgets(sendBuff,1025,res);
               write(connfd,&res,sizeof(res)); 
                
                if ((send(connfd,sendBuff,strlen(sendBuff),0))== -1) 
                {
                     fprintf(stderr, "Failure Sending Message\n");
                     close(connfd);
                     break;
                }

              //printf("Server:Msg being sent: %s\nNumber of bytes sent: %d\n",sendBuff, strlen(sendBuff));

       
      close(connfd); //Close Connection Socket   
      sleep(1);
    } //End of Inner While...
 
 
  return 0;
}
