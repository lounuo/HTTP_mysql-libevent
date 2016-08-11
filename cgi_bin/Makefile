BIN=math_cgi
OBJ=math_cgi.c

$(BIN):$(OBJ)
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -rf $(BIN) output

.PHONY:output
output:$(BIN)
	cp -f math_cgi ../output/cgi_bin
	
