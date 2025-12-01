.PHONY:all
all:caculateClient caculateServer

calclient:FuncClient.cc 
	g++ -o $@ $^ -std=c++11 -lpthread

calserver:FuncServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean 
clean:
	rm -f caculateClient caculateServer