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
	server_addr.sin_addr.s_addr=inet_addr("0,0,0,0");//本机全部ip   或者使用htonl(INADDR_ANY)
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		perror("socket error"),exit(0);
	
	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
		perror("bind error"),exit(0);
	
	if(listen(sockfd,10)<0)
		perror("listen error"),exit(0);
	
	printf("http server is running on port %d\n",ntohs(server_addr.sin_port));
	
	return sockfd;
}

void not_found(){
	
}

void method_get(char* url,int connectfd){
	FILE* file_point=fopen(url,"r");
	if(file_point==NULL)
		not_found();
	else{
		char buf[1024];
		fgets(buf,1024,file_point);
		while(!feof(file_point)){
			send(connectfd,buf,strlen(buf),0);
			fgets(buf,1024,file_point);
		}
	}
	fclose(file_point);
}

void method_head(){
	
}

void method_not_create(){
	
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
		method_head();
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
		
		printf("NEW LINK");
		
		if(pthread_create(&pthread_id,0,request,(void*)&connectfd)<0)
			perror("pthread_create error"),exit(0);
		
		
		
	}
	
	
	return 0;
}