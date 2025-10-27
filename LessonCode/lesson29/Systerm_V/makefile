.PHONY:all
all: processA processB

processA:processA.cc
	g++ -o $@ $^ -std=c++11
processB:processB.cc
	g++ -o $@ $^ -std=c++11


.PHONY:clean
clean:
	rm -f processA processB