CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk #walk1

walk: walk.cpp gzepeda.cpp jgarcia2.cpp jzamora2.cpp bascencio.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a gzepeda.cpp jzamora2.cpp jgarcia2.cpp bascencio.cpp -Wall -Wextra $(LFLAGS) -owalk

#JOSE: used for testing
#walk1: walk1.cpp jzamora2.cpp gzepeda.cpp jgarcia2.cpp 
#	g++ $(CFLAGS) walk1.cpp libggfonts.a jzamora2.cpp gzepeda.cpp jgarcia2.cpp -Wall -Wextra $(LFLAGS) -owalk1


clean:
	rm -f walk
	#rm -f walk1
	rm -f *.o

