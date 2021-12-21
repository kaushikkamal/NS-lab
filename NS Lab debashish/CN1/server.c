#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<time.h>
#include<signal.h>

#define BUF_SIZE 31
#define MAX_CON 10

void validate_args(int,char**,int*);

typedef struct{
	char name[20];
	int quantity;
	long last_sold;
}fruit_t;
fruit_t* load_fruits(int*);
int purchase(char*,fruit_t*,int);

//Linked List that holds unique users that made successful transaction
struct node_t{
    char addr[20];
	struct node_t* next;
};
typedef struct node_t node_t;
node_t* head=NULL;
void insert(struct in_addr,in_port_t);
int search(struct in_addr,in_port_t);

void server_exit(int);

int handle_connection(int c_fd,fruit_t* fruits,int no_fruits,int un_conn,struct sockaddr_in *cad){
	char count=0,buf[BUF_SIZE],order[BUF_SIZE],i;

	sprintf(buf,"%d",un_conn);
	if(write(c_fd,buf,strlen(buf))<0){
		perror(" Failed sending customer count");
	}
	if(read(c_fd,order,BUF_SIZE)<0){
		perror("Failed getting request");
		close(c_fd);
		return 0;
	}

	for(i=0;i<strlen(order);i++) order[i]=tolower(order[i]);
	
	i=purchase(order,fruits,no_fruits);
	if(i>=0){ //here i holds quantity left in storage
		strcpy(buf,"no_quan");
		if(write(c_fd,buf,7)<0){
			perror("Failed sending message 1");
		}
		sleep(1);
		sprintf(buf,"%d",i);
		if(write(c_fd,buf,strlen(buf))<0){ //send the quanity
			perror("Failed sending message 2");
		}
		if(i==0)
			printf("\nOut of stock!\nOrder %s failed\n\n",order);
	}
	else{
		if(i==-2){
			strcpy(buf,"no_fruit");
			if(write(c_fd,buf,8)<0)
				perror("Failed sending message 3");
		}
		else{ //i==-1, success
			strcpy(buf,"done");
			if(write(c_fd,buf,4)<0)
				perror("Failed sending message 4");
			if(!search(cad->sin_addr,cad->sin_port)){
				insert(cad->sin_addr,cad->sin_port);
				return 1;
			}
		}
	}

	return 0;
}

int main(int argc,char** argv){
    int fd,port_no,client_fd,client_len,i,fruit_count;
	int session_unique_conn=0; //hold the number of unique connections in this session
	struct sockaddr_in sad,cad;
	fruit_t *fruit_list;

	validate_args(argc,argv,&port_no);

	fruit_list=load_fruits(&fruit_count);
	
	fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){
        perror("Failed creating socket!");
		exit(1);
    } 

	memset((char*)&sad,0,sizeof(sad));
	memset((char*)&cad,0,sizeof(cad));
	//defining the server address
	sad.sin_family=AF_INET;
	sad.sin_port=htons(port_no);
	sad.sin_addr.s_addr=inet_addr("127.0.0.1");

	//bind this socket to the server address
	if(bind(fd,(struct sockaddr*) &sad,sizeof(sad))<0){
		perror("Failed binding server address");
		close(fd);
		exit(1);
	} 

	signal(SIGINT,server_exit);

	//set this socket to listen for connections with queue length 10
	listen(fd, 10);
	printf("Server created\n");
	
	while(1){
		if((client_fd=accept(fd,(struct sockaddr*) &cad,&client_len))<0){
			perror("Failed accepting connection");
			continue;
		}
		session_unique_conn+=handle_connection(client_fd,fruit_list,fruit_count,session_unique_conn,&cad);
	}
	close(fd);
}

int purchase(char* request,fruit_t* fruits,int count){
	int i,q_int;
	char quantity[5],j,status,len=strlen(request);
	char ret;

	for(i=0;i<count;i++){
		status=1;
		for(j=0;(j<len)&&(j<strlen(fruits[i].name));j++){
			if(request[j]=='.')
				break;
			if(request[j]!=fruits[i].name[j]){
				status=0;
				break;
			}
		}
		if(status){
			status=0;
			int t=0;
			for(j=0;j<len;j++){
				if(!status){
					if(request[j]=='.')
							status=1;
					}
				else{
					quantity[t++]=request[j];
				}
			}
			q_int=atoi(quantity);
			//printf("<%s,%d>",quantity,q_int);fflush(stdout);
			if(fruits[i].quantity<q_int)
				return fruits[i].quantity; //failure, return quantity on storage
			else{
				fruits[i].quantity-=q_int;
				fruits[i].last_sold=time(NULL);
				return -1;//success
			}
		}
	}
	return -2; //failure, fruit not found
}

fruit_t* load_fruits(int* count){
	//load fruits from file
	FILE* fp;
	char line[101],i,len,quan[10];
	int j=0,k=0,l;
	fruit_t *fruits;

	printf("Loading fruits\n");
	if((fp=fopen("fruits","r"))==NULL){
		perror("Failed loading fruits from file");
		exit(1);
	}

	fgets(line,100,fp); //get the no of fruits
	*count=atoi(line);
	if((fruits=(fruit_t*)malloc((*count)*sizeof(fruit_t)))==NULL){
		perror("Failed parsing fruit file");
		exit(1);
	}

	while ((fgets(line,100,fp)!=NULL)&&(k<*count)){
		len=strlen(line);
		for(i=0;i<len-1;i++)
			if(line[i]==' ') //fruit_name and quantity separated by space
				break;
		for(j=i+1,l=0;j<len;j++,l++) //get the fruit count
			quan[l]=line[j];
		line[i]=0;

		strcpy(fruits[k].name,line);
		fruits[k].quantity=atoi(quan);
		fruits[k].last_sold=0;
		k++;
	}
	fclose(fp);
	return fruits;
}

void validate_args(int argc,char** argv,int* pn){
	if(argc<2){
		printf("No Port Number Provided!\n");
		exit(1);
	}
	
	int tmp=atoi(argv[1]);
    if(tmp<=0){ //port no must be greater than 0
		printf("Invalid port no %s\n",argv[1]);
		exit(1);
    }
    *pn = tmp;
}

void insert(struct in_addr add,in_port_t port){
	node_t* new_node=(node_t*)malloc(sizeof(node_t)),*cur_node;
	sprintf(new_node->addr,"%s:%d",inet_ntoa(add),ntohs(port));
	new_node->next=NULL;
	if(!head)
		head=new_node;
	else{
		for(cur_node=head;cur_node->next!=NULL;cur_node=cur_node->next);
		cur_node->next=new_node;
	}
}
int search(struct in_addr add,in_port_t port){
	if(head==NULL) return 0;
	char to_search[20];
	sprintf(to_search,"%s:%d",inet_ntoa(add),ntohs(port));
	node_t* cur_node;
	for(cur_node=head;cur_node!=NULL;cur_node=cur_node->next)
		if(strcmp(to_search,cur_node->addr)==0)
			return 1;
	return 0;
}

void server_exit(int sig_no){
	char choice;
	node_t *cur_node,*pre_node=NULL;
	printf("\nServer Existing");
	printf("\nview all unique succesful users (y/n)");
	scanf(" %c",&choice);
	if(choice=='y'||choice=='Y'){
		if(head==NULL)
			printf("No connections made!");
		else{
			for (cur_node = head; cur_node!= NULL; cur_node = cur_node->next){
				printf("%s\n",cur_node->addr);
			}
		}
	}

	//free memory
	cur_node=head;
	while(cur_node!=NULL){
		pre_node=cur_node;
		cur_node=cur_node->next;
		free(pre_node);
	}
	exit(0);
}
