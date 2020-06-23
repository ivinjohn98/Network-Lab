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
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1,i;
	int addrlen = sizeof(address);
	FILE *fptr;
	char buffer[1024]={0},buffer1[1024] = {0};
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

	fptr = fopen("RollS.txt","w");
	while(1)
	{
		read(new_socket,buffer,1024);
		if(!strcmp(buffer,"quit1234"))
			break;
		fprintf(fptr,"%s",buffer);
	}
	fclose(fptr);
	fptr = fopen("RollS.txt","r");
	fgets(buffer,1024,fptr);
	for(i=0;i<6;i++)
		buffer1[i]=buffer[i+1];
	send(new_socket,buffer1,1024,0);
	fclose(fptr);

	close(new_socket);
	close(server_fd);
   	return 0;
}

int client()
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

	fptr = fopen("RollC.txt","r");
	while(fgets(buffer,1024,fptr)!= NULL)
	{
		send(sock,buffer,1024,0);
	}
	send(sock,"quit1234",1024,0);
	fclose(fptr);
	read(sock,buffer,1024);
	fptr =fopen("NumberC.txt","w");
	fprintf(fptr,"%s",buffer);
	fclose(fptr);

	close(sock);
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

