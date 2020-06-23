#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 1234
#define PORT2 1235
void error(char* msg)
{
	perror(msg);
	exit(1);
}

FILE* file_modify(char filename[40])
{
	FILE *fp1, *fp2;
        char c,line[10];
        fp1 = fopen(filename, "r");
	fgets(line, sizeof(line), fp1);
	c=fgetc(fp1);
        fp2 = fopen("copy.c", "w");
	while(fgets(line,sizeof(line),fp1) != NULL)
		fputs(line,fp2);
	fclose(fp1);
	fclose(fp2);
	remove(filename);
        rename("copy.c", filename);
	fp1 = fopen(filename,"r");
	return fp1;
}


int main()
{
	int server_fd, new_socket,new_socket2,server_fd2;
	struct sockaddr_in address,address2;
	int i,opt=1,opt2=1;
	int addrlen = sizeof(address);
	int addrlen2 = sizeof(address2);
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
		error("bind failed1"); 
	if(listen(server_fd, 3) < 0) 
	        error("listen"); 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
		error("accept");

	if((server_fd2 = socket(AF_INET,SOCK_STREAM,0))==0)
		error("socket failed");
	if(setsockopt(server_fd2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt2,sizeof(opt2))) 
    		error("setsockopt");
	address2.sin_family = AF_INET;
	address2.sin_addr.s_addr = INADDR_ANY; 
	address2.sin_port = htons( PORT2 ); 
    	if(bind(server_fd2, (struct sockaddr *)&address2, sizeof(address2))<0) 
		error("bind failed"); 
	if(listen(server_fd2, 3) < 0) 
	        error("listen"); 
	if ((new_socket2 = accept(server_fd2, (struct sockaddr *)&address2, (socklen_t*)&addrlen2))<0) 
		error("accept");
	fptr=fopen("RollS.txt","r");
	int c1,c2;
	c1 = c2 = 0;
	while(1)
	{
		if((recv(new_socket,buffer,1024,MSG_DONTWAIT)>0) && !c1)
		{
			printf("c1\n");
			if(!strcmp(buffer,"end"))
			{
				c1 = 1;
				if(c1 && c2)
					break;
			}
			fgets(buffer1,1024,fptr);
			send(new_socket,buffer1,1024,0);
			fclose(fptr);
			fptr = file_modify("RollS.txt");
		}
		if((recv(new_socket2,buffer,1024,MSG_DONTWAIT)>0) && !c2)
		{
			printf("c2\n");
			if(!strcmp(buffer,"end"))
			{
				c2 =1;
				if(c1 && c2)
					break;
			}
			fgets(buffer1,1024,fptr);
			send(new_socket2,buffer1,1024,0);
			fclose(fptr);
			fptr = file_modify("RollS.txt");
		}
		bzero(buffer, 1024);
	}
	fclose(fptr);
	close(new_socket);
	close(new_socket2);
	close(server_fd);
	close(server_fd2);
   	return 0;
}

