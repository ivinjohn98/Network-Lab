#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <fstream>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 20021
#define PORT0 20020
#define PORT3 20023
using namespace std;
void error(char const* msg)
{
	perror(msg);
	exit(1);
}

void host_port(int portno,FILE *fptr)
{
	int listen_fd, conn_fd;
	struct sockaddr_in address;
	int opt = 1,childpid;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char const *serv_er = "<SERVER TIME>: FILE NOT FOUND AT CURRENT WORKING DIRECTORY";
	if((listen_fd = socket(AF_INET,SOCK_STREAM,0))==0)
		error("socket failed");
	if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt))) 
    		error("setsockopt");
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( portno ); 
    	if(bind(listen_fd, (struct sockaddr *)&address, sizeof(address))<0) 
		error("bind failed"); 
	if(listen(listen_fd, 3) < 0) 
	        error("listen");
	if ((conn_fd = accept(listen_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
		error("accept");
	if(portno==PORT0)
	{
		while(fgets(buffer,1024,fptr)!= NULL)
		{
			send(conn_fd,buffer,1024,0);
		}
		send(conn_fd,"quit1234",1024,0);
		fclose(fptr);
		close(conn_fd);
		close(listen_fd);
	}
	if(portno==PORT3)
	{
		send(conn_fd,serv_er,strlen(serv_er),0);
		close(conn_fd);
		close(listen_fd);
	}
}

void connect_port(int portno,char *filename)
{
	struct sockaddr_in address; 
	int sock = 0; 
	struct sockaddr_in serv_addr; 
	FILE *fptr;
	char buffer[1024] = {0},*reqsent,modify[1024]= "client-";
	char const *req = "RETR ";
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		error("Socket creation error");
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(portno);       
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
		error("Invalid address/ Address not supported");
    	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("Connection Failed");

	filename = strcat(modify,filename);
	if(portno == PORT0)
	{
		fptr = fopen(filename,"w");
		while(1)
		{
			recv(sock,buffer,1024,0);
			if(!strcmp(buffer,"quit1234"))
				break;
			fprintf(fptr,"%s",buffer);
		}
		fclose(fptr);
	}
	if(portno == PORT3)
	{
		recv(sock,buffer,1024,0);
		printf("%s",buffer);
	}
	close(sock);
}

int server()
{
	int listen_fd, conn_fd;
	struct sockaddr_in address;
	int opt = 1,childpid;
	int addrlen = sizeof(address);
	FILE *fptr;
	char buffer[1024] = {0},c;
	char const *con0 = "CONN 20020",*con3 = "CONN 20023",*con4 = "THANK YOU, ALL CONNECTIONS SUCCESSFULLY TERMINATED";
	if((listen_fd = socket(AF_INET,SOCK_STREAM,0))==0)
		error("socket failed");
	if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt))) 
    		error("setsockopt");
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
    	if(bind(listen_fd, (struct sockaddr *)&address, sizeof(address))<0) 
		error("bind failed"); 
	if(listen(listen_fd, 3) < 0) 
	        error("listen"); 
//start your code here

	while(1)
	{
		if ((conn_fd = accept(listen_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			error("accept");
		if(!(childpid=fork()))
		{
			close(listen_fd);
			while(1)
			{
				recv(conn_fd,buffer,1024,0);
				if(!(childpid=fork()))
				{
					if(strncmp(buffer,"RETR",4)==0)
					{
						if((fptr=fopen(buffer+5,"r"))!=NULL)
						{
							send(conn_fd,con0,strlen(con0),0);
							host_port(PORT0,fptr);
						}
						else
						{
							send(conn_fd,con3,strlen(con3),0);
							host_port(PORT3,NULL);
						}
						bzero(buffer,1024);
					}
					close(conn_fd);
					return 0;
				}
				else
				{
					if(!strcmp(buffer,"GOODBYE SERVER"))
					{
						send(conn_fd,con4,strlen(con4),0);
						close(conn_fd);
						//kill(0,SIGINT);
						return 0;
					}
				}
			}
		}
		close(conn_fd);
	}
}

int client()
{
	struct sockaddr_in address; 
	int sock = 0,childpid,i; 
	struct sockaddr_in serv_addr; 
	FILE *fptr;
	char buffer[1024] = {0},filename[1024],*reqsent,req[] = "RETR ",gdby[] ="GOODBYE SERVER";
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Socket creation error");
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT);       
	//serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
		error("Invalid address/ Address not supported");
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("Connection Failed");
	while(1)
	{
		fgets(filename, 1024, stdin);
		if(!(childpid=fork()))
		{
			if(strcmp(filename,"exit\n"))
			{
				while(buffer[i]!='\n')
					i++;
				buffer[i]='\0';
				strcpy(buffer,req);
				reqsent = strcat(buffer,filename);
				send(sock,reqsent,strlen(reqsent),0);
				reqsent = NULL;
				bzero(buffer,1024);
				recv(sock,buffer,1024,0);
				if(!strcmp(buffer,"CONN 20020"))
				{
					connect_port(PORT0,filename);
				}
				if(!strcmp(buffer,"CONN 20023"))
				{
					connect_port(PORT3,filename);
				}
				bzero(buffer,1024);
			}
			close(sock);
			return 0;
		}
		else
		{
			if(!strcmp(filename,"exit\n"))
			{
				reqsent = strcpy(buffer,gdby);
				send(sock,reqsent,strlen(reqsent),0);
				recv(sock,buffer,1024,0);
				printf("%s\n",buffer);
				close(sock);
				kill(0,SIGINT);
				return 0;
			}
		}
	}
	close(sock);
	return 0;
}

int main(int argc, char** argv)
{
	int x;
	if(!strcmp(argv[1],"server"))
		x = server();
	if(!strcmp(argv[1],"client"))
		x = client();	
	return x;
}

