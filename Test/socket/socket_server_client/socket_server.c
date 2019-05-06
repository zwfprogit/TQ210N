#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MYPORT 8887
#define QUEUE 20
#define BUFFER_SIZE 1024

int main()
{
	// 定义socket 创建SOCKET   判定用何种协议族， 用TCP/UDP类型， 针对第二个参数设置不同协议
	int server_sockfd = socket(AF_INET,SOCK_STREAM,0);
	//printf("server_sockfd:%d\n",server_sockfd);
	
	// 定义sockaddr
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family=AF_INET;
	server_sockaddr.sin_port = htons(MYPORT);
	server_sockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	// bind,成功返回0，出错返回-1
	// 链接IP PORT  该函数用于给socket绑定一个地址   电话号码分配
	if(bind(server_sockfd,(const struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))== -1){
		perror("bind");
		exit(1);
	}

	//listen,成功返回0，出错返回-1
	//监听   该函数在服务端编程会用到，用于监听某个端口，至于是哪个就看你传入进去的第一个参数设置的是哪一个了，
	//第二个参数用于指定等待连接建立的socket队列的最大长度    电话线路连接
	if(listen(server_sockfd,QUEUE)== -1){
		perror("listen");
		exit(1);
	}
	
	//客户端套接字
	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_sockaddr;
	socklen_t length = sizeof(client_sockaddr);
	
	// 阻塞直到有客户端连接，成功返回非负描述字，出错返回-1
	int conn = accept(server_sockfd,(struct sockaddr *)&client_sockaddr,&length);
	if(conn < 0){
		perror("accept");
		exit(1);
	}
	//printf("server client_sockaddr.sin_addr.s_addr[%d]\n",client_sockaddr.sin_addr.s_addr);
	while(1)
	{
		memset(buffer,0x00,sizeof(buffer));
		int len=recv(conn, buffer, sizeof(buffer), 0);
		
		if(strcmp(buffer,"exeit\n") == 0){
			break;
		}
		fputs(buffer,stdout);
		send(conn,buffer,len,0);
	}
	close(conn);
	close(server_sockfd);
	return 0;
}