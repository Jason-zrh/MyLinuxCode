dynamic-lib = libmymethod.so 
static-lib = libmymath.a

.PHONY:all
all:$(dynamic-lib) $(static-lib)


$(static-lib):mymath.o
	ar -rc $@ $^
mymath.o:mymath.c
	gcc -c $^

$(dynamic-lib):myprint.o mylog.o
	gcc -shared -o $@ $^
myprint.o:myprint.c
	gcc -fPIC -c $^
mylog.o:mylog.c
	gcc -fPIC -c $^


.PHONY:clean
clean:
	rm -f *.o *.a *.so mylib


