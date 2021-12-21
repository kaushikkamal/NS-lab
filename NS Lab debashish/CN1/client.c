#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#define BUF_SIZE 31

int validate_args(int,char**,int*);
void Quit(int,int);

int main(int argc,char** argv){
    int fd,port_no,i;
	char buf[BUF_SIZE];
    struct sockaddr_in sad;

    validate_args(argc,argv,&port_no);
	
    if((fd=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("Failed creating socket!");
		exit(1);
	}
       
	//defining the server address
	memset((char*)&sad,0,sizeof(sad));
	sad.sin_family=AF_INET;
	sad.sin_port=htons(port_no);
	sad.sin_addr.s_addr=inet_addr("127.0.0.1");

	if(connect(fd,(struct sockaddr*)&sad,sizeof(sad))<0){
		perror("(  failed connecting to server");
		//Quit(1,fd);
		close(fd);
		exit(1);
	}

	memset(buf,0,BUF_SIZE);
	if(read(fd,buf,BUF_SIZE)<0){ //receving unique customer count
		perror(" failed  getting Customer Count");
	}
	else{
		printf("Customers Number:%s\n",buf);
	}

	sprintf(buf,"%s.%s",argv[2],argv[3]);
	if(write(fd,buf,BUF_SIZE)<0){
		perror("Sending Failed");
		//Quit(1,fd);
		close(fd);
	    exit(1);
	}
	
	//waiting for server response on request
	memset(buf,0,BUF_SIZE);
	if(read(fd,buf,BUF_SIZE)<0){
		perror("error getting response");
		//Quit(1,fd);
		close(fd);
		exit(1);
	}

	//handling server responces
	if(strcmp(buf,"no_fruit")==0)
		printf("Requested fruit %s is not sold!\n",argv[2]);
	else if(strcmp(buf,"done")==0)
		printf("Order of fruit %s with quantity %s is placed\n",argv[2],argv[3]);
	
	else if(strcmp(buf,"no_quan")==0){
		//unable to get the required quantity, server sends the quantity next
		memset(buf,0,BUF_SIZE);
		if(read(fd,buf,BUF_SIZE)<0)
			perror("Failed getting quantity");
		else{
			i=atoi(buf);
			printf(" Purchase failed!\nOnly %d %s available\n",i,argv[2]);
		}
	}
	else printf("Unknown Response!");
	close(fd);
}

int validate_args(int argc,char** argv,int* pn){
    if(argc<4){
		printf("Invalid number of arguments!\n Order format below\n");
		printf("./client port_no fruit_name quantity\neg ./client 1025 apple 5\n");
		exit(1);
    }

    int tmp=atoi(argv[1]);
    if(tmp<=0){ //port no must be greater than 0
		printf("Invalid port no %s\n",argv[1]);
		exit(1);
    }
    *pn = tmp;

    tmp=atoi(argv[3]); //checking quantity
    if(tmp<=0){
		printf("Invalid quantity %s\n",argv[3]);
		exit(1);
    }
}

//void Quit(int exit_code,int fd){
//	close(fd);
//	exit(exit_code);
//}
