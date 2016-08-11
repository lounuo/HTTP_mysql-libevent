#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#define _BACKLOG_ 10
#define _SIZE_ 10240
#define _DF_PATH_ "htdocs/mysql_response.html"
#define _MAX_NUM_ 20

typedef struct data_buf
{
	int _fd;                      //存放文件描述符
	char _buf[2*_SIZE_];          //存放写回的数据
	int _cgi;                     //cgi
	char _method[_SIZE_];         //方法
	char _path[_SIZE_];           //路径
	char _query_string[_SIZE_];   //参数
	int _content_length;          //正文长度
	int _err_num;                 //状态码
}data_buf_t, *data_buf_p;

void Usage(const char *argv);
void print_log(int err_no, const char *fun, int line);
int StartUp(char *ip, int port);
void clear_head(int sock);
int get_line(int sock, char *buf, ssize_t size);
int accept_request(data_buf_p data);
//void* thread_run(void *sock);
void error_response(int sock, int err_num);
int GetContentLength(int sock);
//void send_response(int sock, char* path, size_t size);
void send_response(data_buf_p data, size_t size);
//void exec_response(int sock, const char *mathod, const char *path, const char *query_str);
void exec_response(data_buf_p data);
void epoll_server(int sock);
