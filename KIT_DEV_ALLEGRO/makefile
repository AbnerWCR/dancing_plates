PASTINHA_CONJUNTA=../
CC = $(PASTINHA_CONJUNTA)\MINGW\bin\gcc
PATH_ALLEGRO=$(PASTINHA_CONJUNTA)\allegro-5.0.10-mingw-4.7.0
LIBS=-lallegro-5.0.10-monolith-mt

all: main.exe
	
main.exe: main.o
	$(CC) -O2 -o main.exe maintp.o -L $(PATH_ALLEGRO)\lib $(LIBS)


main.o: main.c
	$(CC) -O2 -I $(PATH_ALLEGRO)\include -c main.c 	
