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
#include<ctype.h>

#define PORT1 20001
#define PORT2 20006
#define BACKLOG 5
#define LENGTH 512 


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void file_modify(char filename[40])
{
	FILE *fp1, *fp2;
        char c,line[10];
        //asks user for file name
        //open file in read mode
        fp1 = fopen(filename, "r");
	//printf("error");
	if(fgets(line, sizeof(line), fp1)==NULL)
		//printf("\n%s\n",line);
                return;
        fp2 = fopen("copy.c", "w");
	c = fgetc(fp1);
        while (c != EOF) {
	  c = fgetc(fp1);
          if(isprint(c)||(c=='\n'))
	    fputc(c, fp2);
        }
	//putc(c, fp2);
        //close both the files.
        fclose(fp1);
        fclose(fp2);
        //remove original file
        remove(filename);
        //rename the file copy.c to original name
        rename("copy.c", filename);
        //return 0;
}


int main ()
{
	/* Defining Variables */
	int sockfd1,sockfd2; 
	int nsockfd1,nsockfd2; 
	int num,i,j;
	int sin_size; 
	struct sockaddr_in addr_local1, addr_local2; /* client addr */
	struct sockaddr_in addr_remote; /* server addr */
	char revbuf[LENGTH],c[10]; // Receiver buffer
        int con1_stat=0,con2_stat=0;

	/* Get the Socket file descriptor */
	if((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor1. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Obtaining socket descriptor1 successfully.\n");
	
	if((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor2. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Obtaining socket descriptor2 successfully.\n");


	/* Fill the client socket address struct */
	addr_local1.sin_family = AF_INET; // Protocol Family
	addr_local1.sin_port = htons(PORT1); // Port number
	addr_local1.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
	printf("%d",addr_local1.sin_addr.s_addr);
	bzero(&(addr_local1.sin_zero), 8); // Flush the rest of struct

	addr_local2.sin_family = AF_INET; // Protocol Family
	addr_local2.sin_port = htons(PORT2); // Port number
	addr_local2.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
	printf("%d",addr_local2.sin_addr.s_addr);
	bzero(&(addr_local2.sin_zero), 8); // Flush the rest of struct

	/* Bind a special Port */
	if( bind(sockfd1, (struct sockaddr*)&addr_local1, sizeof(struct sockaddr)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to bind port %d. (errno = %d)\n",PORT1, errno);
		exit(1);
	}
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT1);

	if( bind(sockfd2, (struct sockaddr*)&addr_local2, sizeof(struct sockaddr)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to bind port %d. (errno = %d)\n",PORT2, errno);
		exit(1);
	}
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT2);

	/* Listen remote connect/calling */
	if(listen(sockfd1,BACKLOG) == -1)
	{
		fprintf(stderr, "ERROR: Failed to listen port %d. (errno = %d)\n",PORT1, errno);
		exit(1);
	}
	else
		printf ("[Server] Listening the port %d successfully.\n", PORT1);

	if(listen(sockfd2,BACKLOG) == -1)
	{
		fprintf(stderr, "ERROR: Failed to listen Port %d. (errno = %d)\n",PORT2, errno);
		exit(1);
	}
	else
		printf ("[Server] Listening the port %d successfully.\n", PORT2);

	int success = 0;
	while(success == 0)
	{
		sin_size = sizeof(struct sockaddr_in);

		/* Wait a connection, and obtain a new socket file despriptor for single connection */
		if ((nsockfd1 = accept(sockfd1, (struct sockaddr *)&addr_remote, &sin_size)) == -1) 
		{
		    fprintf(stderr, "ERROR: Obtaining new Socket Despcritor. (errno = %d)\n", errno);
			exit(1);
		}
		else 
			{
			 con1_stat=1;
			 printf("[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));
			}
		
		if ((nsockfd2 = accept(sockfd2, (struct sockaddr *)&addr_remote, &sin_size)) == -1) 
		{
		    fprintf(stderr, "ERROR: Obtaining new Socket Despcritor. (errno = %d)\n", errno);
			exit(1);
		}
		else 
			{
			con2_stat=1;
			printf("[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));
			}

		
                FILE *fptr;
		int msg_recvd1 = 0, msg_recvd2 = 0;
		char* fs_name = "RollS.txt";
	
		while(1)
		{
			if((msg_recvd1 = recv(nsockfd1, revbuf, LENGTH, MSG_DONTWAIT)) > 0)
			{
				
    				if ((fptr = fopen(fs_name, "r")) == NULL)
    				{
        				printf("Error! opening file");
        				// Program exits if file pointer returns NULL.
        				break;         
    				}
				
				if(c[0] = getc(fptr)==EOF)
				{
					printf("End of file!");
					fclose(fptr);
					if(send(nsockfd1, "CON CLOSED", sizeof("CON CLOSED"), 0) < 0)
					{
			       			fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
			       			break;
					}
					close(nsockfd1);
					printf("[Server] Connection with Client1 closed at %d port.\n", PORT1);
					con1_stat=0;
					if(con1_stat==0 && con2_stat==0)
						break;
				}
				else
				{
					rewind(fptr);
	    				// reads text until newline 
	    				//fscanf(fptr,"%[^\n]", c);
					if(fgets(c, sizeof(c), fptr) != NULL)
					  if(strlen(c)==9)
	    					printf("\nData from the file1:%s\n", c);
					fclose(fptr);
					if(send(nsockfd1, c, sizeof(c), 0) < 0)
					{
			       			fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
			       			break;
					}
	    				
					file_modify(fs_name);
				}
				bzero(revbuf, LENGTH);
				bzero(c,sizeof(c));
				msg_recvd1 = 0;
				msg_recvd2 = 0;
			}
			/*if(msg_recvd <0)
			{
				if (errno == EAGAIN)
	        		{
	                		printf("recv() timed out.\n");
	            		}
	            		else
	            		{
	                		fprintf(stderr, "recv() failed due to errno = %d\n", errno);
					break;
	            		}

			}*/
			if((msg_recvd2 = recv(nsockfd2, revbuf, LENGTH, MSG_DONTWAIT)) > 0)
			{
				if ((fptr = fopen(fs_name, "r")) == NULL)
    				{
        				printf("Error! opening file");
        				// Program exits if file pointer returns NULL.
        				break;         
    				}
				

				if(c[0] = getc(fptr)==EOF)
				{
					printf("End of file!");
					fclose(fptr);
					if(send(nsockfd2, "CON CLOSED", sizeof("CON CLOSED"), 0) < 0)
					{
			       			fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
			       			break;
					}
					close(nsockfd2);
					printf("[Server] Connection with Client2 closed at %d port.\n", PORT2);
					con2_stat=0;
					if(con1_stat==0 && con2_stat==0)
						break;
				}
				else
				{
					rewind(fptr);
	    				// reads text until newline 
	    				//fscanf(fptr,"%[^\n]", c);
					if(fgets(c, sizeof(c), fptr) != NULL)
					  if(strlen(c)==9)
	    					printf("\nData from the file1:%s\n", c);
					fclose(fptr);
					if(send(nsockfd2, c, sizeof(c), 0) < 0)
					{
			       			fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
			       			break;
					}
	    				
					file_modify(fs_name);
				}

    				bzero(revbuf, LENGTH);
				bzero(c,sizeof(c));
				msg_recvd1 = 0;
				msg_recvd2 = 0;
			}
			/* if(msg_recvd1 <0 || msg_recvd2 < 0)
			{
				if (errno == EAGAIN)
	        		{
	                		//printf("recv() timed out.\n");
	            		}
	            		else
	            		{
	                		fprintf(stderr, "recv() failed due to errno = %d\n", errno);
					//break;
	            		}

			} */
			
			
		}
		 

		    printf("\n CONNECTION TO BOTH THE CLIENT TERMINATED SUCESSFULLY!\n"); 
		    success = 1;
		    //close(nsockfd1);
		    //close(nsockfd2);	
		    //printf("[Server] Connection with Client closed. Server will wait now...\n");
		    //while(waitpid(-1, NULL, WNOHANG) > 0);
		
	}
}



