#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#define PORT 1234
using namespace std;

void error(char const* msg)
{
	perror(msg);
	exit(1);
}

char* modifiedTime(char *filePath) 
{
	struct stat attrib;
	stat(filePath, &attrib);
	char date[10];
	strftime(date, 20, "%d-%m-%y", localtime(&(attrib.st_ctime)));
	//printf("The file %s was last modified at %s\n",filePath, date);
	return strdup(date);
}

int receiving(int socket,char str[],int size,int j)
{
	int i=0;
	while(1)
	{
		recv(socket,str+i,1,0);
		if(str[i]=='\0')
			return 1;
	}
}

int server()
{
	int listen_fd, conn_fd;
	struct sockaddr_in address;
	int opt = 1,childpid,i,j,k;
	int addrlen = sizeof(address);
	FILE *fptr;
	char command[2048],buffer[2028],filename[2048],c;
	char const *c1 ="HTTP/1.1 200 ok\n";
	char const *c2 ="HTTP/1.1 404\n";
	char const *c3 ="HTTP/1.1 400\n";
	char const *c4 ="HTTP/1.1 403\n";
	char const *c5 ="HTTP/1.1 405\n";
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
//main part of the code 
	bzero(buffer,2048);
	bzero(command,2048);
	while(1)
	{
		if ((conn_fd = accept(listen_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			error("accept");
		if(!(childpid = fork()))
		{
			close(listen_fd);
			bzero(command,2048);
			while(recv(conn_fd,command,2048,0)>0)
			{
				i = 0;
				while(command[i++]!='\0');
				if(!(command[i-3]=='1' && command[i-4]=='.' 
					&& command[i-5]=='1' && command[i-6]=='/' && command[i-7]=='P' 
					&& command[i-8]=='T' && command[i-9]=='T' && command[i-10]=='H'))
				{
					if(send(conn_fd,c3,2048,0)<=0)
						error("send");
				}
				else
				{
					i = 0;
					while(command[i++]!=' ');
					k = i;
					while(command[i++]!=' ')
					{
						if(command[i-1]=='/')
							j = i;
					}
					command[i-1] = '\0';
					if(!strncmp(command,"GET",3))
					{
						if((i-j)==1)
						{
							fptr = fopen("index.html","r");
							if(send(conn_fd,c1,2048,0)<=0)
								error("send");
							while(fgets(buffer,2048,fptr)!= NULL)
							{
								send(conn_fd,buffer,2048,0);
							}
							send(conn_fd,"quit1234",2048,0);
							fclose(fptr);
						}
						else if((fptr = fopen(command+5,"r"))==NULL)
						{
							if(send(conn_fd,c2,2048,0)<=0)
								error("send");
						}
						else if(command[j]=='1' && command[j+1]=='1')
						{
							if(send(conn_fd,c4,2048,0)<=0)
								error("send");
						}
						else
						{
							if(send(conn_fd,c1,2048,0)<=0)
								error("send");
							while(fgets(buffer,2048,fptr)!= NULL)
							{
								send(conn_fd,buffer,2048,0);
							}
							send(conn_fd,"quit1234",2048,0);
							fclose(fptr);
						}
					}
					else if(!strncmp(command,"HEAD",4))
					{
//printf("\n%s\n",command+5);
						if((fptr = fopen(command+6,"r"))==NULL)
						{
							if(send(conn_fd,c2,2048,0)<=0)
								error("send");
						}
						else if(command[j]=='1' && command[j+1]=='1')
						{
							if(send(conn_fd,c4,2048,0)<=0)
								error("send");
						}
						else
						{
							if(send(conn_fd,c1,2048,0)<=0)
								error("send");
							if(send(conn_fd,modifiedTime(command+4),2048,0)<=0)
								error("send");
						}
						fclose(fptr);
					}
					else if(!strncmp(command,"PUT",3))
					{
						/*if(command[j]=='1' && command[j+1]=='1')
						{
							if(send(conn_fd,c5,strlen(c5),0)<=0)
								error("send");
						}
						else
						{*/
							fptr = fopen(command+j,"w");
							while(1)
							{
								bzero(buffer,2048);
								if(recv(conn_fd,buffer,2048,0)<=0)
									error("recv");
								if(!strcmp(buffer,"quit1234"))
									break;
								fprintf(fptr,"%s",buffer);
							}
							fclose(fptr);
						//}
					}
					else if(!strncmp(command,"DELETE",6))
					{
						if(command[j]=='1' && command[j+1]=='1')
						{
							if(send(conn_fd,c5,2048,0)<=0)
								error("send");
						}
						else if(!remove(command+8))
						{
							if(send(conn_fd,c1,2048,0)<=0)
								error("send");
						}
						else
						{
							if(send(conn_fd,c2,2048,0)<=0)
								error("send");
						}
					}
					else
					{
						if(send(conn_fd,c5,2048,0)<=0)
								error("send");
					}
				}
			}
			error("recv");
		}
		close(conn_fd);
	}
}

int client()
{
	struct sockaddr_in address; 
	int sock = 0,i,j; 
	struct sockaddr_in serv_addr; 
	FILE *fptr;
	char command[2048],buffer[2048],c;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Socket creation error");
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT);
    	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
		error("Invalid address/ Address not supported");
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("Connection Failed");
	while(1)
	{
		bzero(command,2048);
		fgets(command, 2048, stdin);
		if(send(sock,command,2048,0)<=0)
			error("send");
		i = 0;
		while(command[i++]!='\0');
		if(!(command[i-3]=='1' && command[i-4]=='.' 
			&& command[i-5]=='1' && command[i-6]=='/' && command[i-7]=='P' 
			&& command[i-8]=='T' && command[i-9]=='T' && command[i-10]=='H'))
		{
			bzero(buffer,2048);
			if(recv(sock,buffer,2048,0)<=0)
				error("recv");
			cout<<buffer;
		}
		else
		{
			i = 0;
			while(command[i++]!=' ');
			while(command[i++]!=' ');
			{
				if(command[i-1]=='/')
					j = i;
			}
			command[i-1] = '\0';
			if(!strncmp(command,"GET",3))
			{
				bzero(buffer,2048);

				if(recv(sock,buffer,2048,0)<=0)
					error("recv");
				cout<<buffer;
				if(!strcmp(buffer,"HTTP/1.1 200 ok\n"))
				{
					//fptr = fopen(command+j,"w");
					while(1)
					{
						bzero(buffer,2048);
						if(recv(sock,buffer,2048,0)<=0)
							error("recv");
						if(!strcmp(buffer,"quit1234"))
							break;
						cout<<buffer;
						//fprintf(fptr,"%s",buffer);
					}
					//fclose(fptr);
				}
			}
			else if(!strncmp(command,"HEAD",4))
			{
				bzero(buffer,2048);
				if(recv(sock,buffer,2048,0)<=0)
					error("recv");
				cout<<buffer;
				if(!strcmp(buffer,"HTTP/1.1 200 ok\n"))
				{
					bzero(buffer,2048);
					if(recv(sock,buffer,2048,0)<=0)
						error("recv");
					cout<<buffer;
				}
			}
			else if(!strncmp(command,"PUT",3))
			{
				/*if(command[j]=='1' && command[j+1]=='1')
				{
					if(recv(sock,buffer,strlen(buffer),0) <= 0)
						error("recv");
				}
				else
				{*/
					fptr = fopen(command+4,"r");		
					while(fgets(buffer,2048,fptr)!= NULL)
					{
						send(sock,buffer,sizeof(buffer),0);
					}
					send(sock,"quit1234",2048,0);
					fclose(fptr);
				//}
			}
			else if(!strncmp(command,"DELETE",6))
			{
				bzero(buffer,2048);
				if(recv(sock,buffer,2048,0)<=0)
					error("recv");
				cout<<buffer;
			}
			else
			{
				bzero(buffer,2048);
				if(recv(sock,buffer,2048,0)<=0)
					error("recv");
				cout<<buffer;
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

