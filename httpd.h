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
#include <unistd.h>
#include <fcntl.h>

#define _BACKLOG_ 10
#define _SIZE_ 10240
#define _DF_PATH_ "htdocs/response.html"
#define _MAX_NUM_ 20

void Usage(const char *argv);
void print_log(int err_no, const char *fun, int line);
int StartUp(char *ip, int port);
void clear_head(int sock);
int get_line(int sock, char *buf, ssize_t size);
void accept_request(int sock);
void* thread_run(void *sock);
void error_response(int sock, int err_num);
int GetContentLength(int sock);
void send_response(int sock, char* path, size_t size);
void exec_response(int sock, const char *mathod, const char *path, const char *query_str);

