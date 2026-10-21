FLAGS = -Wall -g -ansi -pedantic -std=c++11

ifeq ($(MAP), 1)
	FLAGS += -DMAP
endif
ifeq ($(MAP), -1)
	FLAGS += -UMAP
endif



clean : 
	-rm *.o $(objects) 