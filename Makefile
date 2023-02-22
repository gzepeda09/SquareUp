CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk

walk: walk.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -owalk jzamora2.cpp

clean:
	rm -f walk
	rm -f *.o

