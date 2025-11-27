.PHONY:ALL
ALL:udpserver udpclient

udpclient:UdpClient.cc
	g++ -o $@ $^ -std=c++11

udpserver:main.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f udpserver udpclient