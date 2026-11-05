test : test.o 
	gcc test.o -o test

test.o: test.c
	gcc -c test.c 

server :
	gcc ClientServeur/Serveur/server2.c -o server

client :
	gcc ClientServeur/Client/client2.c -o client

clean : 
	-rm *.o $(objects) server client test