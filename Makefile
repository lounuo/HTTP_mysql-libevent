ROOT_PATH=$(shell pwd)
LDFLAGS=-levent#-lpthread#-static
FLAGS=#-D_DEBUG_#-g
CC=gcc
BIN=httpd

CGI_BIN=cgi_bin
MYSQL=cgi_bin/MySql


SRC=$(shell ls *.c)
OBJ=$(SRC:.c=.o)

$(BIN):$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o:%.c
	$(CC) -c $< $(FLAGS)

.PHONY:clean
clean:
	@rm -rf *.o $(BIN) output
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make clean;\
		cd -;\
	done
	@for name in `echo $(MYSQL)`;\
	do\
		cd $$name;\
		make clean;\
		cd --;\
	done

.PHONY:cgi
cgi:
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make;\
		cd -;\
	done

.PHONY:mysql
mysql:
	@for name in `echo $(MYSQL)`;\
	do\
		cd $$name;\
		make;\
		cd --;\
	done

.PHONY:output
output:$(BIN) cgi mysql
	@mkdir -p output/log
	@cp -rf htdocs output
	@mkdir -p output/cgi_bin
	@mkdir -p output/cgi_bin/MySql
	@cp -f httpd output/
	@cp -f start.sh output/
	@cp -rf conf output
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done
	@for name in `echo $(MYSQL)`;\
	do\
		cd $$name;\
		make output;\
		cd --;\
	done
