.PHONY:all
all:calclient calserver

calclient:clientCaculate.cc 
	g++ -o $@ $^ -std=c++11 -lpthread

calserver:serverCaculate.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean 
clean:
	rm -f calclient calserver