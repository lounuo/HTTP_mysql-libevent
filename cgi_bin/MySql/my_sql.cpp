#include "my_sql.h"

//构造函数
server_database::server_database()
	:_host("127.0.0.1")
	,_user("root")
	,_password("")
	,_db("HTTP_server")
	,_port(3306)
	,_res(NULL)
{}

//析构函数
server_database::~server_database()
{
	mysql_free_result(_res);
	mysql_close(&_mysql);
	//std::cout<<"Close database..."<<std::endl;
}

//连接数据库
int server_database::Connect_database()
{
	mysql_init(&_mysql);

	if(mysql_real_connect(&_mysql, _host.c_str(), _user.c_str(), _password.c_str(), _db.c_str(), _port, NULL, 0) == 0)
		return -1;
	else
		return 0;
}


int server_database::_op_fun(std::string op_query)
{
	return mysql_query(&_mysql, op_query.c_str());
}

//查询数据库所有信息
bool server_database::Select()
{
	int flag = 0;
	std::string op_query = "select * from Person";
	if(_name != "")
	{
		op_query += " where Name=\"";
		op_query += _name;
		op_query += "\"";
		flag = 1;
	}
	if(_sex != "")
	{
		if(flag == 0)
			op_query += " where Sex='";
		else
			op_query += " and Sex='";

		if(_sex[0] == 'l')
			op_query += "F";
		else
			op_query += "M";
		op_query += "'";
		flag = 1;
	}
	if(_age != "")
	{
		if(flag == 0)
			op_query += " where Age=";
		else
			op_query += " and Age=";
		op_query += _age;
		flag = 1;
	}
	if(_school != "")
	{
		if(flag == 0)
			op_query += " where School=\"";
		else
			op_query += " and School=\"";
		op_query += _school;
		op_query += "\"";
		flag = 1;
	}
	//if(flag == 0)
	//	op_query = "select * from Person";


	if(_op_fun(op_query))//查询数据表
	{
		Error("select error");
		//std::cout<<"select failed "<<mysql_error(&_mysql)<<std::endl;
		return false;
	}
	//else
	//	std::cout<<"select success..."<<std::endl;

	_res = mysql_store_result(&_mysql);//获取结果集
	if(_res == NULL)
	{
		Error("select error");
		//std::cout<<"get result failed..."<<std::endl;
		return false;
	}
	printf("\t<h2>number of dataline returned: %d</h2>\n", mysql_affected_rows(&_mysql));
	
	MYSQL_FIELD *fd;//字段列数组
	char field[1024][1024];//用于存放字段名
	for(int i = 0; fd = mysql_fetch_field(_res); ++i)//获取字段名
	{
		strcpy(field[i], fd->name);
	}

	int j = mysql_num_fields(_res);//获取列数
	for(int i = 0; i < j; ++i)
	{
		printf("%s\t", field[i]);//将每一列的字段名打印出来
	}
	printf("<br/>");

	MYSQL_ROW column;//表示数据行的列
	while(column = mysql_fetch_row(_res))//获取每一行的内容并将其打印出来
	{
		for(int i = 0; i < j; ++i)
			printf("%s\t", column[i]);
		printf("<br/>");
	}
	return true;
}


bool server_database::Insert()
{
	std::string op_query = "insert into Person (Name, Sex, Age, School, Registration_time) values (\"";
	
	if(_name == "")
	{
		Error("name");
		return false;
	}
	if(_sex == "")
	{
		Error("sex");
		return false;
	}
	if(_age == "")
	{
		Error("age");
		return false;
	}
	if(_school == "")
	{
		Error("school");
		return false;
	}

	op_query += _name;
	op_query += "\", '";
	if(_sex[0] == 'l')
		op_query += "F";
	else
		op_query += "M";
	op_query += "', ";
	op_query += _age;
	op_query += ", \"";
	op_query += _school;
	op_query += "\", NOW())";

	if(_op_fun(op_query))
	{
		Error("insert error");
		//std::cout<<"Insert failed..."<<mysql_error(&_mysql)<<std::endl;
		return false;
	}
	else
	{
		//std::cout<<"Insert success..."<<std::endl;
		//(*this).Success("insert");
		return true;
	}
}

bool server_database::Delete()
{
	int flag = 0;
	std::string op_query = "delete from Person";
	if(_name != "")
	{
		op_query += " where Name=\"";
		op_query += _name;
		op_query += "\"";
		flag = 1;
	}
	if(_sex != "")
	{
		if(flag == 0)
			op_query += " where Sex='";
		else
			op_query += " and Sex='";

		if(_sex[0] == 'l')
			op_query += "F";
		else
			op_query += "M";
		op_query += "'";
		flag = 1;
	}
	if(_age != "")
	{
		if(flag == 0)
			op_query += " where Age=";
		else
			op_query += " and Age=";
		op_query += _age;
		flag = 1;
	}
	if(_school != "")
	{
		if(flag == 0)
			op_query += " where School=\"";
		else
			op_query += " and School=\"";
		op_query += _school;
		op_query += "\"";
		flag = 1;
	}
	if(flag == 0)
		op_query = "delete * from Person";


	//std::cout<<"op_query: "<<op_query<<std::endl;
	//Select();
	if(_op_fun(op_query))
	{
		Error("delete error");
		//std::cout<<"Delete failed..."<<mysql_error(&_mysql)<<std::endl;
		return false;
	}
	else
	{
		//std::cout<<"Delete success..."<<std::endl;
		//(*this).Success("delete");
		return true;
	}
}

bool server_database::Modify()
{

	return true;
}

void server_database::Error(const char* arg)
{
	const char* picture = "/home/lounuo/HTTP/htdocs/yeyuanxinzhizhu.jpg";
	int out_fd = open(picture, O_RDONLY);
	struct stat st;
	stat(picture, &st);

	if(strlen(arg) < 7)
	{
		printf("HTTP/1.1 400 Bad Request\r\n\r\n");
		printf("<html>\n");
		printf("<head>\n");
		printf("ERROR INFORMATION!\n");
		printf("</head>\n");
		printf("<body>\n");
	
		if(strcmp(arg, "op") == 0)
			printf("Operation can not be NULL, please return back to input again......\n");
		if(strcmp(arg, "name") == 0)
			printf("Name can not be NULL, please return back to input again......\n");
		if(strcmp(arg, "sex") == 0)
			printf("Sex can not be NULL, please return back to input again......\n");
		if(strcmp(arg, "age") == 0)
			printf("Age can not be NULL, please return back to input again......\n");
		if(strcmp(arg, "school") == 0)
			printf("School can not be NULL, please return back to input again......\n");

		sendfile(1, out_fd, NULL, st.st_size);

		printf("</body>\n");
		printf("</html>\n");
		close(out_fd);
		return;
	}

	printf("HTTP/1.1 500 Internal Server Error\r\n\r\n");
	printf("<html>\n");
	printf("<head>\n");
	printf("SERVER ERROR!\n");
	printf("</head>\n");
	printf("<body>\n");
	if(strcmp(arg, "op error") == 0)
		printf("Operation error, please return back to choose again......\n");
	if(strcmp(arg, "select error") == 0)
		printf("server select information error, please return back to try again......\n");
	if(strcmp(arg, "insert error") == 0)
		printf("server insert information error, please return back to try again......\n");
	if(strcmp(arg, "delete error") == 0)
		printf("server delete information error, please return back to try again......\n");
	if(strcmp(arg, "connect error") == 0)
		printf("server connect databases error, please return back to try again......\n");

	sendfile(1, out_fd, NULL, st.st_size);

	printf("</body>\n");
	printf("</html>\n");
	close(out_fd);
}

void server_database::Success(const char* arg)
{
	printf("HTTP/1.1 200 OK\r\n\r\n");
	printf("<html>\n");
	printf("<head>\n");
	if(strcmp(arg, "select") == 0)
	{
		printf("</head>\n");
		printf("<body>\n");
		Select();
	}
	else if(strcmp(arg, "insert") == 0)
	{
		printf("\t<h1>Insert Success......</h1>\n");
		printf("</head>\n");
		printf("<body>\n");
		Select();
		_name = _sex = _age = _school = "";
		Select();
	}
	else if(strcmp(arg, "delete") == 0)
	{
		printf("\t<h1>Delete Success......</h1>\n");
		printf("</head>\n");
		printf("<body>\n");
		Select();
		_name = _sex = _age = _school = "";
		Select();
	}

	printf("</body>\n");
	printf("</html>\n");
}













