.PHONY:all
all: mycommond otherExe 

mycommond:mycommond.c
	gcc -o $@ $^ -std=c99

otherExe:otherExe.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f mycommond otherExe
