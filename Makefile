FLAGS = -Wall -g -ansi -pedantic -std=c++11

ifeq ($(MAP), 1)
	FLAGS += -DMAP
endif
ifeq ($(MAP), -1)
	FLAGS += -UMAP
endif

server :
	gcc ClientServeur/Serveur/server2.c -o server

client :
	gcc ClientServeur/Client/client2.c -o client

clean : 
	-rm *.o $(objects) server client