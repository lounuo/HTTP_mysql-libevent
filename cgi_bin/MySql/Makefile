ROOT_PATH=$(shell pwd)
CC=g++

BIN=my_sql
MYSQL_SRC=my_sql.cpp
MAIN_SRC=main_mysql.cpp
MYSQL_OBJ=my_sql.o
MAIN_OBJ=main_mysql.o
MYSQL=$(ROOT_PATH)/mysqllib/include

INCLUDE=-I$(MYSQL)
LIB=$(ROOT_PATH)/mysqllib/lib
#LDFLAGS=/home/lounuo/HTTP/cgi_bin/MySql/mysqllib/lib/libmysqlclient.a
LDFLAGS=-lmysqlclient

LIB_LINK=-L$(LIB)

$(BIN):$(MYSQL_OBJ) $(MAIN_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIB_LINK)

$(MYSQL_OBJ):$(MYSQL_SRC)
	$(CC) -c $< $(INCLUDE)
$(MAIN_OBJ):$(MAIN_SRC)
	$(CC) -c $< $(INCLUDE)

.PHONY:clean
clean:
	rm -f *.o $(BIN)

.PHONY:output
output:$(BIN)
	cp -rf my_sql ../../output/cgi_bin/MySql
