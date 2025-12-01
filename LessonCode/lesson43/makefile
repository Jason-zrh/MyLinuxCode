.PHONY:all
all:caculateClient caculateServer

caculateClient:FuncClient.cc 
	g++ -o $@ $^ -std=c++11 -lpthread

caculateServer:FuncServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean 
clean:
	rm -f caculateClient caculateServer