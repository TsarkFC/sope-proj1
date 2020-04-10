simpledu: simpledu.o reg.o rec.o utils.o
	gcc simpledu.o reg.o rec.o utils.o -Wall -o simpledu

simpledu.o: simpledu.c
	gcc -c simpledu.c

reg.o: reg.c reg.h 
	gcc -c reg.c

rec.o: rec.c rec.h 
	gcc -c rec.c

utils.o: utils.c utils.h 
	gcc -c utils.c

clean:
	rm *.o simpledu