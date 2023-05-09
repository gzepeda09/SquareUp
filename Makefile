CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lopenal -lalut -lm -lglut #-lXrandr

all: SquareUp

walk: walk.cpp gzepeda.cpp jgarcia2.cpp jzamora2.cpp bascencio.cpp box.cpp image.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a gzepeda.cpp jzamora2.cpp jgarcia2.cpp bascencio.cpp box.cpp image.cpp -Wall -Wextra $(LFLAGS) -oSquareUp

clean:
	rm -f SqaureUp
	rm -f *.o

