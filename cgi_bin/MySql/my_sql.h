#pragma once

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "mysql.h"

class server_database
{
	int _op_fun(std::string op_query);
	public:
		server_database();
		~server_database();
		
		int Connect_database();
		bool Select();
		bool Insert();
		bool Delete();
		bool Modify();
		void Error(const char* arg);
		void Success(const char* arg);

	public:
		std::string _op;
		std::string _name;
		std::string _sex;
		std::string _age;
		std::string _school;

	private:
		MYSQL _mysql;
		std::string _host;
		std::string _user;
		std::string _password;
		std::string _db;
		unsigned int _port;
		MYSQL_RES *_res;
};






