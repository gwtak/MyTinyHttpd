#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h> 
#include<string.h>
#include<pthread.h>
#include<unistd.h>//close()


int sock_init(){
	int sockfd;
	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(4000);
	server_addr.sin_addr.s_addr=inet_addr("0.0.0.0");//本机全部ip  或者使用htonl(INADDR_ANY) 不要把‘.’写成‘,’
	
	if((sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
		perror("socket error"),exit(0);
	
	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
		perror("bind error"),exit(0);
	
	if(listen(sockfd,5)<0)
		perror("listen error"),exit(0);
	
	printf("http server is running on port %d\n",ntohs(server_addr.sin_port));
	
	return sockfd;
}

void not_found(int connectfd){
	char buf[1024];
	sprintf(buf, "HTTP/1.1 404 NOT FOUND\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "Server:@gwtak");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "is unavailable or nonexistent.\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(connectfd, buf, strlen(buf), 0);
}

void method_head(int connectfd){
	char buf[1024];
	sprintf(buf, "HTTP/1.1 200 OK\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(connectfd, buf, strlen(buf), 0);
}


void method_get(char* url,int connectfd){
	if(url[strlen(url)-1]=='/')
		strcat(url,"index.html");
	
	FILE* file_point=fopen(url,"r");
	if(file_point==NULL)
		not_found(connectfd);
	else{
		char buf[1024];
		method_head(connectfd);
		fgets(buf,1024,file_point);
		while(!feof(file_point)){
			send(connectfd,buf,strlen(buf),0);
			fgets(buf,1024,file_point);
		}
	}
	fclose(file_point);
}

void method_not_create(int connectfd){
	char buf[1024];
	sprintf(buf, "HTTP/1.1 501 Method Not Create\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING);
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><HEAD><TITLE>Method Not Create\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "</TITLE></HEAD>\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
	send(connectfd, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(connectfd, buf, strlen(buf), 0);
}

void* request(void* arg){
	int connectfd=*(int*)arg;
	char buf[2048];
	char method[255];
	char url[255];
	
	if(recv(connectfd,buf,2048,0)<0)
		perror("recv error"),exit(0);
	
	int i=0;//标记buf读取位置
	while(buf[i]!=' '){
		method[i]=buf[i];
		i++;
	}
	
	while(buf[i]==' ')i++;
	
	int j=0;
	while(buf[i]!=' '){
		url[j]=buf[i];
		i++;
		j++;
	}
	
	if(method=="GET"){
		method_get(url,connectfd);
	}
	else if(method=="HEAD"){
		method_head(connectfd);
	}
	else{
		method_not_create();
	}
	
	close(connectfd);
}

int main(){
	int sockfd=sock_init();
	int connectfd;
	struct sockaddr_in client;
	pthread_t pthread_id;
	socklen_t client_size=sizeof(client);//必须如此使用
	
	while(1){
		if((connectfd=accept(sockfd,(struct sockaddr*)&client,&client_size))<0)
			perror("accept error"),exit(0);

		printf("NEW LINK\n");
		
		if(pthread_create(&pthread_id,0,request,(void*)&connectfd)<0)
			perror("pthread_create error"),exit(0);
		
	}
	
	return 0;
}