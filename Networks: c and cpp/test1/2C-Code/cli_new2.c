#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 20006
#define LENGTH 10 


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	/* Variable Definition */
	int sockfd; 
	int nsockfd;
	char revbuf[LENGTH]; 
	char buff;
	struct sockaddr_in remote_addr;

	/* Get the Socket file descriptor */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
		exit(1);
	}

	/* Fill the socket address struct */
	remote_addr.sin_family = AF_INET; 
	remote_addr.sin_port = htons(PORT); 
	inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr); 
	bzero(&(remote_addr.sin_zero), 8);

	/* Try to connect the remote */
	if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
		exit(1);
	}
	else 
		printf("[Client] Connected to server at port %d...ok!\n", PORT);

	/* Send Signal to Server */
	
	char* fr_name = "RollNumberC2.txt";
	
	while(1)
	{

	while ((buff = getchar()) != '\n'); 
	if(send(sockfd, &buff, sizeof(char), 0) < 0)
	{
		fprintf(stderr, "ERROR: Failed to send to server. (errno = %d)\n", errno);
		exit(1);
	}
	FILE *fr = fopen(fr_name, "a");
	if(fr == NULL)
		printf("File %s Cannot be opened.\n", fr_name);
	else
	{
		bzero(revbuf, LENGTH); 
		int fr_block_sz = 0;

		if((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
	    	{
			if(strcmp(revbuf,"CON CLOSED")==0)
			  {
			  printf("\n CONNECTION CLOSED FROM SERVER\n END OF FILE");
			  close (sockfd);
			  //printf("[Client] Connection lost.\n");
			  return (0);
			  break;
			  }
			else
			{

			printf("Data From Server: %s",revbuf);
			fputs(revbuf, fr);
			fputs("\n", fr);
			}
			
	    	}
		
	    fclose(fr);	
	    //printf("Ok received from server!\n");
	    
	}
	}
	//fclose(fr);
	close (sockfd);
	printf("[Client] Connection lost.\n");
	return (0);
}
