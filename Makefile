CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk jowalk

walk: walk.cpp gzepeda.cpp jgarcia2.cpp jzamora2.cpp bascencio.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a gzepeda.cpp jzamora2.cpp jgarcia2.cpp bascencio.cpp -Wall -Wextra $(LFLAGS) -owalk

clean:
	rm -f walk jowalk
	rm -f *.o

