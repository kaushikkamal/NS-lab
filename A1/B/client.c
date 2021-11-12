// calc client

#include <stdio.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <string.h>

#include <arpa/inet.h>

int main(int argc, char *argv[])

{

  int clientSocket;

  char buffer[1024];

  int result;

  struct sockaddr_in serverAddr, clientAddr;

  socklen_t addr_size;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  serverAddr.sin_family = AF_INET;

  serverAddr.sin_port = htons(7891);

  serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

  fgets(buffer, 1024, stdin);

  // strcpy(buffer, argv[1]);

  send(clientSocket, buffer, 1024, 0);

  recv(clientSocket, &result, 4, 0);

  printf("\n From server: %d\n", result);

  return 0;
}
