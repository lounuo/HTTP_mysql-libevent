#include "my_sql.h"

void handle_data(std::string query_string)
{
	if(strcmp(query_string.c_str(), "") != 0)
	{
		server_database ser_db;
		if(query_string[0] != 'o')//如果没有操作则错误
		{
			ser_db.Error("op");
			return;
		}
		//std::cout<<"query_string: "<<query_string<<std::endl;

		int index = 0;
		std::vector<char *> v_query;

		for(int i = 0; i < query_string.size(); ++i)
		{
			if(query_string[i] == '=')
			{
				v_query.push_back(&(query_string[i+1]));
				++index;
			}
			else if(query_string[i] == '&')
			{
				query_string[i] = '\0';
				if(index == 2)
				{//如果age没有被选择，那么在参数中就不会出现，则这里需要手动将其添加置为""
					if(query_string[i+1] != 's')
					{
						v_query.push_back(&(query_string[i]));
						++index;
					}
				}
			}
		}
		//检查参数是否被正确截取
		//for(int i = 0; i < index; ++i)
		//{
		//	std::cout<<i<<": "<<v_query[i]<<std::endl;
		//}

		//将获取的参数依次赋值
		ser_db._op = v_query[0];
		ser_db._name = v_query[1];
		ser_db._sex = v_query[2];
		ser_db._age = v_query[3];
		ser_db._school = v_query[4];
		//std::cout<<"op:"<<ser_db._op<<std::endl;
		//std::cout<<"name:"<<ser_db._name<<std::endl;
		//std::cout<<"sex:"<<ser_db._sex<<std::endl;
		//std::cout<<"age:"<<ser_db._age<<std::endl;
		//std::cout<<"school:"<<ser_db._school<<std::endl;

		if(ser_db.Connect_database() < 0)//连接数据库
		{
			ser_db.Error("connect error");
			//std::cout<<"connect database failed..."<<std::endl;
		}
		//else
		//	std::cout<<"connect database success..."<<std::endl;

		switch(*(v_query[0]))
		{
			case 'i':
				{
					if(ser_db.Insert())
						ser_db.Success("insert");
						////////////
					break;
				}
			case 'd':
				{
					if(ser_db.Delete())
						ser_db.Success("delete");
					break;
				}
			case 's':
				{
					ser_db.Success("select");
					break;
				}
			case 'm':
				{
					if(ser_db.Modify())
						ser_db.Success("modify");
					break;
				}
			default:
				{
					ser_db.Error("op error");
					break;
				}
		}


	}
}


int main()
{
	char *method;
	char *query_string;
	char *content_length;
	char buf[1024];

	memset(buf, '\0', sizeof(buf));

	if(getenv("METHOD"))
	{
		method = getenv("METHOD");
	}
	//printf("exec_method:%s\n", method);

	if(strcasecmp(method, "GET") == 0)
	{
		if(getenv("QUERY_STRING"))
			query_string = getenv("QUERY_STRING");
		else
			query_string = "";
	}
	else if(strcasecmp(method, "POST") == 0)
	{
		if(getenv("CONTENT_LENGTH"))
		{
			content_length = getenv("CONTENT_LENGTH");

			int len = atoi(content_length);
			//printf("hello len:%d\n", len);
			int index = 0;
			while(len)
			{
				read(0, &buf[index], 1);
				--len;
				++index;
			}
			query_string = buf;
		}
		else
			content_length = "";
	}
	else
	{
		//
	}
	//printf("query string:%s\n",query_string);

	//std::string query_string = "operation=insert&name=baby&sex=lady&age=25&school=XPU";
	//std::string query_string = "operation=delete&name=&sex=&age=0&school=";
	//std::string query_string = "operation=select&name=&age=&school=";
	//std::string query_string = "operation=insert&name=daodao&sex=gentleman&age=18&school=XTU";
	handle_data(query_string);

	return 0;
}
