lib=libmymath.a

$(lib):mymath.o
	ar -rc $@ $^
mymath.o:mymath.c
	gcc -c $^


.PHONY:clean
clean:
	rm -f *.o *.a 

