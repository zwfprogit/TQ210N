#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MYPORT 17806
#define BUFFER_SIZE 1500

int main()
{
	
	// 定义socket
	int client_sockfd = socket(AF_INET,SOCK_STREAM,0);
	//printf("client_sockfd:%d\n",client_sockfd);
	
	// 定义sockaddr
	struct sockaddr_in servaddr;
	memset(&servaddr,0x00,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port = htons(MYPORT);//服务器端口
	servaddr.sin_addr.s_addr = inet_addr("60.205.148.225");//服务器IP
	
	//连接服务器，成功返回0，错误返回-1
	if(connect(client_sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
		perror("connect");
		exit(1);
	}
	printf("client client_sockaddr.sin_addr.s_addr[%d]\n",servaddr.sin_addr.s_addr);

	char sendbuffer[BUFFER_SIZE];
	char recvbuffer[BUFFER_SIZE];
	char login[]={"232301fe4c413935434542463547314c433030303801001e1304140a2022010138393836303242353236313633303030333130360100d3"};
	int l = send(client_sockfd,login,strlen(login),0);
	printf("client send data [%s]\n",login);
	recv(client_sockfd,recvbuffer,sizeof(recvbuffer),0);
	fputs(recvbuffer,stdout);
	while(fgets(sendbuffer,sizeof(sendbuffer),stdin) != NULL)
	{
		int l = send(client_sockfd,sendbuffer,strlen(sendbuffer),0);
		printf("client send data [%s]\n",sendbuffer);
		if(strcmp(sendbuffer,"exit\n") == 0){
			break;
		}
		recv(client_sockfd,recvbuffer,sizeof(recvbuffer),0);
		fputs(recvbuffer,stdout);

		memset(sendbuffer,0x00,sizeof(sendbuffer));
		memset(recvbuffer,0x00,sizeof(recvbuffer));
	}
	close(client_sockfd);
	return 0;
}