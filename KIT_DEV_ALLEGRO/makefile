PASTINHA_CONJUNTA=../
CC = $(PASTINHA_CONJUNTA)\MINGW\bin\gcc
PATH_ALLEGRO=$(PASTINHA_CONJUNTA)\allegro-5.0.10-mingw-4.7.0
LIBS=-lallegro-5.0.10-monolith-mt

all: plates.exe
	
plates.exe: plates.o
	$(CC) -O2 -o plates.exe plates.o -L $(PATH_ALLEGRO)\lib $(LIBS)


plates.o: plates.c
	$(CC) -O2 -I $(PATH_ALLEGRO)\include -c plates.c 	
