#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 1235
void error(char* msg)
{
	perror(msg);
	exit(1);
}
int main()
{
	struct sockaddr_in address; 
	int sock = 0; 
	struct sockaddr_in serv_addr; 
	FILE *fptr;
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		error("Socket creation error");
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT);       
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
		error("Invalid address/ Address not supported");
    	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("Connection Failed");

	fptr =fopen("RollNumberC2.txt","w");
	while(1)
	{
		bzero(buffer,1024);
		fgets(buffer,sizeof(buffer),stdin);
		if(!strcmp(buffer,"end\n"))
		{
			send(sock,buffer,1024,0);
			break;
		}
		send(sock,buffer,1024,0);
		bzero(buffer,1024);
		recv(sock,buffer,1024,0);
		fprintf(fptr,"%s",buffer);
	}
	
	fclose(fptr);
	close(sock);
	return 0;
}

