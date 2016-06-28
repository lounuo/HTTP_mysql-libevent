#include "httpd.h"

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		Usage(argv[0]);
		exit(0);
	}

	int port = atoi(argv[2]);
	char *ip = argv[1];

	int listen_sock = StartUp(ip, port);

	//struct sockaddr_in client;
	//socklen_t client_len = sizeof(client);
	//while(1)
	//{
	//	int client_sock = accept(listen_sock, (struct sockaddr*)&client, &client_len);
	//	if(client_sock < 0)
	//	{
	//		print_log(errno, __FUNCTION__, __LINE__);
	//		continue;
	//	}
	//	char *client_ip = inet_ntoa(client.sin_addr);
	//	int client_port = ntohs(client.sin_port);

	//	printf("get a request...[ip]:%s  [port]:%d\n", client_ip, client_port);

	//	pthread_t tid;
	//	pthread_create(&tid, NULL, thread_run, (void*)client_sock);
	//	pthread_detach(tid);
	//}
	
	epoll_server(listen_sock);
	close(listen_sock);
	return 0;
}










