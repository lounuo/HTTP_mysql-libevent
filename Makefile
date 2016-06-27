ROOT_PATH=$(shell pwd)
LDFLAGS=-lpthread#-static
FLAGS=#-D_DEBUG_#-g
CC=gcc
BIN=httpd

CGI_BIN=cgi_bin

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

.PHONY:cgi
cgi:
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make;\
		cd -;\
	done

.PHONY:output
output:$(BIN) cgi
	@mkdir -p output/log
	@cp -rf htdocs output
	@mkdir -p output/htdocs/cgi_bin
	@cp -f httpd output/
	@cp -f start.sh output/
	@cp -rf conf output
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done
