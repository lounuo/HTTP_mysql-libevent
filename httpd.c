#include "httpd.h"

//格式差错
void Usage(const char *argv)
{
	assert(argv);
	printf("Usage: %s  [ip]  [port]\n", argv);
}

//打印错误信息
void print_log(int err_no, const char *fun, int line)
{
	printf("error: %s   function: %s   line: %d\n", strerror(err_no), fun, line);
}

//创建监听套接字
int StartUp(char *ip, int port)
{
	assert(ip);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		exit(1);
	}

	int op = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip);

	if(bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0)//绑定
	{
		print_log(errno, __FUNCTION__, __LINE__);
		exit(2);
	}

	if(listen(sock, _BACKLOG_) < 0)//监听
	{
		print_log(errno, __FUNCTION__, __LINE__);
		exit(3);
	}

	return sock;
}

//从socket当中读取一行信息
int get_line(int sock, char *buf, ssize_t size)
{
	assert(buf);

	memset(buf, '\0', size);//初始化缓冲区
	char ch;
	int index = 0;

	while((recv(sock, &ch, 1, 0) > 0) && (index < (size-1)))
	{
		if(ch == '\r')//如果ch为\r的时候需要进一步判断下一个字符是否为\n
		{
			recv(sock, &ch, 1, MSG_PEEK);//窥探，并不取出数据
			if(ch == '\n')//如果是，将其取出，此时并不存储\r,而是将\r\n看作一个回车换行\n来存储
				continue;
		}//如果只有\r则直接存储，其他数据非\n也一样
		buf[index++] = ch;
		if(ch == '\n')//判断如果为\n则直接跳出循环，读取完毕一行数据
			break;
	}
	return index;
}

//错误响应
void error_response(int sock, int err_num)
{
	char *error;
	if(err_num == 400)
		error = "HTTP/1.1 400 Bad Request\r\n\r\n";//请求错误
	else if(err_num == 404)
		error = "HTTP/1.1 404 Not Found\r\n\r\n";//资源不存在
	else if(err_num == 405)
		error = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";//方法未定义
	else if(err_num == 408)
		error = "HTTP/1.1 408 Request Timeout\r\n\r\n";//请求超时
	else if(err_num == 500)
		error = "HTTP/1.1 500 Internal Server Error\r\n\r\n";//服务器内部错误

	clear_head(sock);
	printf("error_msg:%s\n", error);
	if(send(sock, error, strlen(error), 0) < 0)//发送状态行
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	char *error_path = "/home/lounuo/HTTP/htdocs/error_response.html";
	int out_fd = open(error_path, O_RDONLY);
	if(out_fd < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	struct stat st;
	if(stat(error_path, &st) < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}

	if(sendfile(sock, out_fd, NULL, st.st_size) < 0)//发送错误页面
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	close(out_fd);
	close(sock);
}

//清除头部信息
void clear_head(int sock)
{
	int ret = 1;
	char buf[1024];
	buf[0] = '\0';
	while((strcmp(buf, "\n") != 0) && ret > 0)
	{
		ret = get_line(sock, buf, sizeof(buf));
	}
}

//发送正确响应
void send_response(data_buf_p data, size_t size)
{
	char *response = "HTTP/1.0 200 OK\r\n\r\n";
	if(send(data->_fd, response, strlen(response), 0) < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	int out_fd = open(data->_path, O_RDONLY);
	if(out_fd < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	if(sendfile(data->_fd, out_fd, NULL, size) < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	close(out_fd);
}

//获取正文长度
int GetContentLength(int sock)
{
	char content_length[_SIZE_/2];
	int cont_len = 0;
	char buf[_SIZE_];
	while(get_line(sock, buf, sizeof(buf)) > 0)
	{
		if(strncmp(buf, "Content-Length:", 15) == 0)
		{
			char tmp[_SIZE_/2];
			strcpy(tmp, &buf[15]);
			cont_len = atoi(tmp);
			printf("content_length:%d\n", cont_len);
			sprintf(content_length, "CONTENT_LENGTH=%s", tmp);
			int ret = putenv(content_length);//导入环境变量content_length
			if(ret == 0)
				printf("put env CONTENT_LENGTH success...\n");
			else
				printf("put env CONTENT_LENGTH failed...\n");
			break;
		}
	}
	return cont_len;
}

//执行可执行文件
void exec_response(data_buf_p data)
{
	char query_str[_SIZE_];
	char method[_SIZE_/2];
	sprintf(query_str, "QUERY_STRING=%s", data->_query_string);
	int ret = putenv(query_str);//导入环境变量query_string
	if(ret == 0)
		printf("put env QUERY_STRING success...\n");
	else
		printf("put env QUERY_STRING failed...\n");
	
	sprintf(method, "METHOD=%s", data->_method);
	ret = putenv(method);//导入环境变量query_string
	if(ret == 0)
		printf("put env METHOD success...\n");
	else
		printf("put env METHOD failed...\n");

	//因为需要执行一个可执行文件，所以应该使用exec进行程序替换
	//而进行程序替换不能使用当前进程，所以需要fork出一个子进程来执行
	int pipe1[2];
	pipe(pipe1);
	int pipe2[2];
	pipe(pipe2);

	pid_t id = fork();
	if(id < 0)//创建进程失败
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	else if(id == 0)//子进程
	{
		close(pipe1[1]);//子进程要从pipe1当中读取数据
		close(pipe2[0]);//在pipe2中写入数据

		dup2(pipe1[0], 0);
		dup2(pipe2[1], 1);

		close(pipe1[0]);
		close(pipe2[1]);

		execl(data->_path, data->_path, NULL);
	}
	else//父进程
	{
		close(pipe1[0]);//父进程要往pipe1当中写入数据
		close(pipe2[1]);//从pipe2中读取数据

		clear_head(data->_fd);

		char ch;
		int con_len = data->_content_length;
		while(con_len > 0)
		{
			if(recv(data->_fd, &ch, 1, 0) > 0)
				write(pipe1[1], &ch, 1);
				
			--con_len;
		}

		memset(data->_buf, '\0', sizeof(data->_buf));
		int index = 0;
		int ret = 0;
		while(ret = read(pipe2[0], &ch, 1))
		{
			if(ret > 0)
				(data->_buf)[index++] = ch;
			else
			{
				print_log(errno, __FUNCTION__, __LINE__);
				break;
			}
		}
		//printf("asejfuhauhsdfhajhdsfjqiowefhasdjfhakjsdfkj\n");

		close(pipe1[1]);
		close(pipe2[0]);
	}
}

//处理远端请求
int accept_request(data_buf_p data)
{
	//这里最好不要用assert进行参数的差错判断，因为有可能套接字的文件描述符恰好为0号文件描述符
	int sock = data->_fd;
	char buf[_SIZE_];
	char request_line[_SIZE_];
	char method[(_SIZE_/2)];
	char url[(_SIZE_/2)];
	char query_string[_SIZE_];
	data->_cgi = 0;

	memset(method, '\0', sizeof(method));
	memset(url, '\0', sizeof(url));
	memset(query_string, '\0', sizeof(query_string));

#ifdef _DEBUG_//debug条件编译进行调试
	while(get_line(sock, buf, sizeof(buf)) > 0)
	{
		printf("%s", buf);
	}
	fflush(stdout);

#else
	if(get_line(sock, request_line, sizeof(request_line)) == 0)//首先获取请求行信息，需要知道请求的方法，这里只考虑GET和POST方法
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return -1;
	}

	printf("%s\n", request_line);//打印出获取的请求行
	int index = 0;
	int line_size = strlen(request_line);
	for(; index < line_size; ++index)
	{
		if(isspace(request_line[index]))//以空格为间隔符获取方法
			break;
		method[index] = request_line[index];
	}

	while((index < line_size) && isspace(request_line[index]))
		index++;//循环结束时的index应该指向的是url信息的开始部位

	//表示已经获取到了方法
	if(strcasecmp(method, "POST") == 0)//如果为POST方法，则使用cgi模式
	{
		data->_cgi = 1;
		int i = 0;
		for(; (request_line[index]!='\0')&&(!isspace(request_line[index])); ++i,++index)
			url[i] = request_line[index];//提取出url
	}
	else if(strcasecmp(method, "GET") == 0)//如果为GET方法，则需要进一步进行判断
	{
		if(index < line_size)
		{
			char *p_query_str = &request_line[index];
			for(; p_query_str < &(request_line[line_size]); ++p_query_str)
			{
				if(*p_query_str == '?')//如果url的字段中含有?，则表示含有参数信息，需要使用cgi模式
				{
					*p_query_str = '\0';//将url中的路径和参数一分为二
					data->_cgi = 1;
				}
				else if(isspace(*p_query_str))
				{
					*p_query_str = '\0';//将url和后面的信息分离开
					break;
				}
			}
			if(data->_cgi == 1)
			{
				int i = index+strlen(&(request_line[index]))+1;//i指向参数部分
				strcpy(query_string, &(request_line[i]));//提取出参数部分
			}
			strcpy(url, &request_line[index]);
		}
	}
	else
	{
		//除了GET和POST之外
		return 405;
	}

	char path[_SIZE_/2] = "/home/lounuo/HTTP";
	if(url[0] == '/')
	{
		strcat(path, url);
	}

	struct stat st;
	if(stat(path, &st) < 0)
	{
		printf("error path:%s\n",path);
		return 404;
	}

	if((st.st_mode & S_IFDIR) != 0)//判断是否为一个文件夹
	{
		strcat(path, _DF_PATH_);//如果是，将其路径更改为默认路径
	}
	else if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))//判断是否为一个可执行文件
	{
		data->_cgi = 1;
	}
	//else
	//{
	//	error_response(sock, 404);//即不是文件也不是文件夹，请求资源不存在
	//	return NULL;
	//}
	//
	strcpy(data->_method, method);
	strcpy(data->_path, path);
	strcpy(data->_query_string, query_string);
	if(data->_cgi == 0)//一定是GET方法且不含参数的请求
	{
		clear_head(sock);
		//send_response(sock, path, st.st_size);//发回响应
	}
	else
	{
		if(strcasecmp(data->_method, "POST") == 0)
		{
			data->_content_length = GetContentLength(sock);
		}
		else
			data->_content_length = 0;
		//exec_response(sock, method, path, query_string);
		exec_response(data);
	}

	//close(sock);
#endif
	return 0;
}

//epoll
void epoll_server(int sock)
{
	int cgi = 0;
	int epoll_fd = epoll_create(256);//创建epoll实例
	if(epoll_fd < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		exit(1);
	}

	//用一个epoll_event结构体向epoll实例中注册需要IO的事件
	struct epoll_event ep_ev;
	ep_ev.events = EPOLLIN;
	ep_ev.data.fd = sock;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ep_ev) < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		exit(2);
	}

	//申请空间用于存放就绪事件
	struct epoll_event evs[_MAX_NUM_];
	int maxnum = _MAX_NUM_;
	int timeout = -1;//以阻塞方式等待，单位为毫秒
	int ret = 0;//用于接收epoll_wait的返回值，也就是就绪事件的个数

	while(1)
	{
		switch((ret = epoll_wait(epoll_fd, evs, maxnum, timeout)))
		{
			case -1://出错
				print_log(errno, __FUNCTION__, __LINE__);
				break;
			case 0://超时
				printf("timeout...\n");
				break;
			default://至少有一个事件就绪
				{
					//printf("IO ready:%d\n", ret);
					int i = 0;
					for(; i < ret; ++i)
					{
						//判断是否为监听套接字就绪，如果是，处理链接请求
						if((evs[i].data.fd == sock) && (evs[i].events & EPOLLIN))
						{
							struct sockaddr_in client;
							socklen_t client_len = sizeof(client);

							int accept_sock = accept(sock, (struct sockaddr*)&client, &client_len);
							if(accept_sock < 0)
							{
								print_log(errno, __FUNCTION__, __LINE__);
								continue;
							}
							char *client_ip = inet_ntoa(client.sin_addr);
							int client_port = ntohs(client.sin_port);
							printf("get a request from client...[ip]:%s  [port]:%d\n", client_ip, client_port);

							ep_ev.events = EPOLLIN;
							ep_ev.data.fd = accept_sock;
							if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_sock, &ep_ev) < 0)
							{
								print_log(errno, __FUNCTION__, __LINE__);
								close(accept_sock);
							}
						}
						else//除了监听套接字之外的IO套接字就绪
						{
							if(evs[i].events & EPOLLIN)//读事件就绪
							{
								data_buf_p _data = (data_buf_p)malloc(sizeof(data_buf_t));
								if(_data == NULL)
								{
									print_log(errno, __FUNCTION__, __LINE__);
									continue;
								}
								_data->_fd = evs[i].data.fd;
								_data->_err_num = accept_request(_data);//读取数据
								
								ep_ev.data.ptr = _data;
								ep_ev.events = EPOLLOUT;
								epoll_ctl(epoll_fd, EPOLL_CTL_MOD, evs[i].data.fd, &ep_ev);
								//printf("read data over\n");
							}
							else if(evs[i].events & EPOLLOUT)//写事件就绪
							{
								data_buf_p _data = (data_buf_p)evs[i].data.ptr;
								if(_data->_err_num == 0)//如果返回值为0，则表示正常退出
								{
									if(_data->_cgi == 0)//cgi=0
									{
										struct stat st;
										if(stat(_data->_path, &st) < 0)
										{
											print_log(errno, __FUNCTION__, __LINE__);
											return;
										}
										send_response(_data, st.st_size);
										//printf("send data over\n");
									}
									else//cgi=1
									{
										send(_data->_fd, _data->_buf, strlen(_data->_buf), 0);
										//printf("exec over\n");
									}
								}
								else if(_data->_err_num > 0)//错误状态码
									error_response(_data->_fd, _data->_err_num);

								if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, _data->_fd, NULL) < 0)
									print_log(errno, __FUNCTION__, __LINE__);
								close(_data->_fd);
								free(_data);
								//printf("all of data over,free\n");
							}
							else
							{}
						}
					}
				}
				break;
		}
	}
}


//data_buf_p _data = (data_buf_p)malloc(sizeof(data_buf_t));
data_buf_p _data;

void write_fc(int write_sock, short event, void *arg)
{
	struct event_base *base = (struct event_base*)arg;

	//data_buf_p _data = (data_buf_p)evs[i].data.ptr;
	if(_data->_err_num == 0)//如果返回值为0，则表示正常退出
	{
		if(_data->_cgi == 0)//cgi=0
		{
			struct stat st;
			if(stat(_data->_path, &st) < 0)
			{
				print_log(errno, __FUNCTION__, __LINE__);
				return;
			}
			send_response(_data, st.st_size);
			//printf("send data over\n");
		}
		else//cgi=1
		{
			send(_data->_fd, _data->_buf, strlen(_data->_buf), 0);
			//printf("exec over\n");
		}
	}
	else if(_data->_err_num > 0)//错误状态码
		error_response(_data->_fd, _data->_err_num);

	//if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, _data->_fd, NULL) < 0)
	//	print_log(errno, __FUNCTION__, __LINE__);
	close(_data->_fd);
	//free(_data);
}

void read_fc(int read_sock, short event, void *arg)
{
	struct event_base *base = (struct event_base*)arg;

	_data = (data_buf_p)malloc(sizeof(data_buf_t));
	if(_data == NULL)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}
	_data->_fd = read_sock;
	_data->_err_num = accept_request(_data);//读取数据

	struct event *write_event;
	write_event = event_new(base, read_sock, EV_WRITE, write_fc, (void*)base);
	event_add(write_event, NULL);
	event_base_dispatch(base);
}

void accept_fc(int listen_sock, short event, void *arg)
{
	struct event_base *base = (struct event_base*)arg;
	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);

	int client_fd = accept(listen_sock, (struct sockaddr*)&client, &client_len);
	if(client_fd < 0)
	{
		print_log(errno, __FUNCTION__, __LINE__);
		return;
	}

	struct event *read_event;
	read_event = event_new(base, client_fd, EV_READ, read_fc, (void*)base);
	event_add(read_event, NULL);
	event_base_dispatch(base);
}










