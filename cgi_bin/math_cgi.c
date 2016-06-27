#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//void echo_result(char **val, char* op, char* res)
void echo_result(char *data1, char *data2, char* op, char* res)
{
	printf("HTTP/1.1 200 OK\r\n\r\n");
	printf("<html>\n");
	printf("<head>\n");
	printf("\t<h1>Calculation result:</h1>\n");
	printf("</head>\n");
	printf("<body>\n");

	//printf("data1:%s data2:%s op:%s res:%s\n", data1, data2, op, res);
	if((strcmp(op, "/") == 0) && (strcmp(data2, "0") == 0))
	{
		printf("\t<p>ERROR: 除数不能为0！</p>");
	}
	else
	{
		printf("\t<p> %s %s %s = %s </p>", data1, op, data2, res);
	}

	printf("</body>\n");
	printf("</html>\n");
}

char* ExtractData(char *data, char *sybom)
{
	if((*sybom == '+') || (*sybom == '-'))
		++data;//提取数据部分
	else if((*data >= '0') && (*data <= '9'))
		*sybom = '+';//如果为正数，确定符号
	else
		data = "0";//如果为非法字符，则数据直接为0

	while(*data == '0')//跳过开头所有的0
		++data;

	char* tmp = data;
	while(*tmp != '\0')
	{
		if(*tmp < '0' || *tmp > '9')//如果数据中间有非法字符，截取前面的数字
		{
			*tmp = '\0';
			break;
		}
		++tmp;
	}

	if(*data == '\0')
		return "0";

	return data;
}

//加法
char* ADD(const char* data1, const char* data2, char sybom1, char sybom2)
{
	size_t size1 = strlen(data1);
	size_t size2 = strlen(data2);
	if(size1 < size2)//使得长度大的数据在前
	{
		const char* tmp = data1;
		data1 = data2;
		data2 = tmp;
		size1 = strlen(data1);
		size2 = strlen(data2);
	}

	char *ret = (char *)malloc(size1+2);
	*ret = sybom1;//不管本来是相加还是相减，符号都和第一个数统一，只需要将两个除去符号的数相加
	
	int index = 1;
	int step = 0;
	for(; index <= size1; ++index)
	{
		int tmp = (data1[size1-index]-'0')+step;//取出前面数据的最后一个数字
		if(index <= size2)
			tmp += (data2[size2-index]-'0');//和后面数据的最后一个数字相加

		ret[size1-index+2] = (tmp%10)+'0';//取得相加的个位数
		step = tmp/10;//进位
	}
	
	ret[1] = step+'0';

	return ret;
}

//减法
char* SUB(const char* data1, const char* data2, char sybom1, char sybom2)
{
	size_t size1 = strlen(data1);
	size_t size2 = strlen(data2);
	char sybom;
	if((size1 < size2) || ((size1 == size2) && (strcmp(data1, data2) < 0)))//使得数据大的数在前
	{
		const char* tmp = data1;
		data1 = data2;
		data2 = tmp;
		size1 = strlen(data1);
		size2 = strlen(data2);
		sybom = sybom2;//前者数据比后者数据小，结果和后者保持一致
	}
	else
		sybom = sybom1;//否则说明前者数据较大或者二者相等，结果符号和其保持一致

	char *ret = (char *)malloc(size1+1);
	*ret = sybom;
	
	int index = 1;
	int step = 0;
	for(; index <= size1; ++index)
	{
		int tmp = (data1[size1-index]-'0'-step);//取出前面数据的最后一个要运算的数字
		if(index <= size2)
			tmp -= (data2[size2-index]-'0');//和后面数据的最后一个数字相减

		if(tmp < 0)
		{
			step = 1;//当相减后的结果小于0时向前借位
			tmp += 10;
		}
		else
			step = 0;

		ret[size1-index+1] = (tmp+'0');//取得相减的个位数
	}

	return ret;
}

//乘法
char* MUL(const char* data1, const char* data2, char sybom1, char sybom2)
{
	//当相乘的两个数中有一个为0时，结果为0
	if((strcmp(data1, "0") == 0) || (strcmp(data2, "0") == 0))
	{
		char *ret = (char *)malloc(1);
		*ret = '0';
		return ret;
	}

	size_t size1 = strlen(data1);
	size_t size2 = strlen(data2);
	if(size1 < size2)//使得长度大的数据在前
	{
		const char* tmp = data1;
		data1 = data2;
		data2 = tmp;
		size1 = strlen(data1);
		size2 = strlen(data2);
	}

	int retlen = (size1+size2+1);//存放结果的空间大小
	char *ret = (char *)malloc(retlen);
	int k = 0;
	for(; k < retlen; ++k)//将结果的每一位都初始化为0
	{
		ret[k] = '0';
	}

	//如果相乘的数据符号相同，则结果为正，不同则结果为负
	if(sybom1 == sybom2)
		*ret = '+';
	else
		*ret = '-';

	int index = 1;
	int offset = 0;//移位
	for(; index <= size2; ++index)
	{
		int tmp = data2[size2-index] - '0';//取后面的数要进行运算的一位
		int step = 0;//进位
		if(tmp == 0)
		{
			++offset;
			continue;
		}
		
		int i = 1;
		for(; i <= size1; ++i)//使得刚刚取得运算的一位和较大数据的每一位进行相乘
		{
			int onebyone = tmp * (data1[size1-i] - '0');
			onebyone += step;
			onebyone += (ret[retlen-offset-i] - '0');
			ret[retlen-offset-i] = (onebyone%10 + '0');
			step = onebyone/10;
		}

		ret[retlen-offset-size1-1] += step;
		++offset;
	}

	return ret;
}

//判断左边数是否比右边数大
int IsLeftStrBig(char *pleft, int lsize, char *pright, int rsize)
{
	if((lsize > rsize) || ((lsize == rsize) && (strncmp(pleft, pright, lsize) >= 0)))
		return 0;

	return -1;
}

//循环相减
char SubLoop(char* pleft, int lsize, char *pright, int rsize)
{
	char ret = '0';
	while(1)
	{
		if(IsLeftStrBig(pleft, lsize, pright, rsize) < 0)//循环相减直到较大的数据不够减去较小的数据为止
			break;

		int ldatalen = lsize-1;
		int rdatalen = rsize-1;
		while(rdatalen >= 0 && ldatalen >= 0)//进行减法
		{
			if(pleft[ldatalen] < pright[rdatalen])
			{
				pleft[ldatalen-1] -= 1;
				pleft[ldatalen] += 10;
			}

			pleft[ldatalen] = pleft[ldatalen]-pright[rdatalen]+'0';
			--ldatalen;
			--rdatalen;
		}

		while((*pleft == '0') && (lsize > 0))//
		{
			++pleft;
			--lsize;
		}

		++ret;
	}
	return ret;
}

//除法
char* DIV(const char* data1, const char* data2, char sybom1, char sybom2)
{
	if(strcmp(data2, "0") == 0)//除数不能为0
		return NULL;

	size_t size1 = strlen(data1);
	size_t size2 = strlen(data2);
	if((size1 < size2) || ((size1 == size2) && (strcmp(data1, data2) < 0)))//如果被除数小于除数，则结果为0
	{
		char *ret = (char *)malloc(1);
		*ret = '0';
		return ret;
	}

	int retlen = (size1+1);//存放结果的空间大小
	char *ret = (char *)malloc(retlen);
	int k = 0;
	for(; k < retlen; ++k)//将结果的每一位都初始化为0
	{
		ret[k] = '0';
	}
	//如果相除的数据符号相同，则结果为正，不同则结果为负
	if(sybom1 == sybom2)
		*ret = '+';
	else
		*ret = '-';

	//拷贝两份数据
	char *pleft = (char *)malloc(size1);
	strcpy(pleft, data1);
	char *ltmp = pleft;
	char *pright = (char *)malloc(size2);
	strcpy(pright, data2);
	char *rtmp = pright;

	//printf("pleft:%s pright:%s\n", pleft, pright);
	int datalen = 1;
	int index = 0;
	for(; index < size1; )
	{
		//if(*pleft == '0')
		//{
		//	++left;
		//	++index;
		//	continue;
		//}
		//比较被除数的前几位，直到找到足以运算的位数为止
		if(IsLeftStrBig(pleft, datalen, pright, size2) < 0)
		{
			++datalen;
			if((index+datalen) > size1)
				break;

			continue;
		}
		else
		{
			ret[index+datalen] = SubLoop(pleft, datalen, pright, size2);//循环相减
			//printf("tmp_ret:%s\n", ret);

			while((*pleft == '0') && (datalen > 0))
			{
				++pleft;
				++index;
				--datalen;
			}

			++datalen;
			if((index+datalen) > size1)
				break;
		}
	}
	free(ltmp);
	free(rtmp);

	return ret;
}

char* AdjustmentData(char *data)
{
	char sybom = *data;
	if((sybom == '-') || (sybom == '+'))
		++data;

	while(*data == '0')
		++data;
	if(*data == '\0')
		return "0";

	if(sybom == '-')
	{
		*(data-1) = '-';
		return (data-1);
	}
	else
	{
		return data;
	}
}

//处理数据
void handle_data(char *query_string)
{
	if(strcmp(query_string, "") != 0)
	{
		char *val[3];

		char *tmp = query_string;
		char *tail = query_string+strlen(query_string)-1;//tail指向query_string的结尾
		int index = 0;
		while(tmp < tail)//分别获取每一个参数放入指针数组p中
		{
			if((*tmp) == '=')
			{
				val[index] = tmp+1;
				++index;
			}
			else if((*tmp) == '&')
				*tmp = '\0';

			++tmp;
		}

		char *op = val[1];//提取出操作符
		val[1] = val[2];
		val[2] = NULL;

		char *data1 = val[0];
		char sybom1 = *data1;//提取data1的符号
		data1 = ExtractData(data1, &sybom1);
		char *data2 = val[1];
		char sybom2 = *data2;//提取data2的符号
		data2 = ExtractData(data2, &sybom2);

		char *res = NULL;
		switch(op[0])//按操作符进行相应的操作
		{
			case 'a':
				{
					if(sybom1 == sybom2)
						res = ADD(data1, data2, sybom1, sybom2);//如果两个符号相同，进行加法
					else
						res = SUB(data1, data2, sybom1, sybom2);//如果两个符号不同，进行减法
					op = "+";
					break;
				}
			case 's':
				{
					if(sybom1 == sybom2)
						res = SUB(data1, data2, sybom1, sybom2);//如果两个符号相同，进行减法
					else
						res = ADD(data1, data2, sybom1, sybom2);//如果两个符号不同，进行加法
					op = "-";
					break;
				}
			case 'm':
				{
					res = MUL(data1, data2, sybom1, sybom2);
					op = "*";
					break;
				}
			case 'd':
				{
					res = DIV(data1, data2, sybom1, sybom2);
					op = "/";
					break;
				}
			default:
				break;
		}

		//if(sybom1 == '-')
		//	memcpy()//

		echo_result(AdjustmentData(val[0]), AdjustmentData(val[1]), op, AdjustmentData(res));
		if(res != NULL)
			free(res);
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

	handle_data(query_string);

	return 0;
}
