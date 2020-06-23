#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 1234
void error(char* msg)
{
	perror(msg);
	exit(1);
}

int server()
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char *auths = "This number belongs to NITC S6/S8 batch";
	char *authu = "OK";
	char buffer[1024]={0};
	if((server_fd = socket(AF_INET,SOCK_STREAM,0))==0)
		error("socket failed");
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt))) 
    		error("setsockopt");
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
    	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
		error("bind failed"); 
	if(listen(server_fd, 3) < 0) 
	        error("listen"); 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
		error("accept");
	while(1)
	{ 
		read(new_socket,buffer,1024);
		if(!strncmp("Hello:b160943cs",buffer,15))
		{
			send(new_socket,auths,strlen(auths),0);
			break;
		}
		else
		{
			send(new_socket,authu,strlen(authu),0);
		}
		bzero(buffer, 1024);
	}
	close(new_socket);
	close(server_fd);
   	return 0;
}

int client()
{
	struct sockaddr_in address; 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char *auths = "This number belongs to NITC S6/S8 batch";
	char *authu = "OK",*text;
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
	printf("Please enter \"Hello:<Roll Number>\"\n");
	while(1)
	{
		scanf("%s",buffer);
		send(sock,buffer,strlen(buffer),0);
		printf("CLIENT:%s\n",buffer);
		memset(buffer,0,1024);
		read(sock,buffer,1024);
		printf("SERVER:%s\n",buffer);
		if(!strcmp(auths,buffer))
		{
			printf("Successfully authentication\n");
			break;
		}
		if(!strcmp(authu,buffer))
			printf("Authenticated unsuccessfully,Plese try again\n");
	}
	return 0;
}

int main(int argc, char** argv)
{
	//if(argc>2)
	//	printf("More than one arguments passed");
	int x;
	if(!strcmp(argv[1],"server"))
		x = server();
	if(!strcmp(argv[1],"client"))
		x = client();	
	return x;
}

