#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 1000

int main(int argc, char *argv[])
{
     int sockfd;
     int len;
     struct sockaddr_in serv_addr;
     int result;
     char buf[] = "GET / HTTP/1.1\n\n\n";
     char get[MAXLINE] = {0};

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = inet_addr("192.168.111.153");
     serv_addr.sin_port = htons(4000);

     len = sizeof(serv_addr);
		
	 printf("before connect\n");
     if( connect(sockfd, (struct sockaddr *)&serv_addr, len) == -1)
    {
      write(stdout,"connect error", 13);
      exit(1);
     }
	
     write(sockfd,buf, sizeof(buf));
     read(sockfd,get, sizeof(get));

     close(sockfd);
     exit(0);
}