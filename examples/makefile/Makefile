all: main main_static

main.o: main.cpp
	g++ -std=c++11 -c -o $@ `pkg-config --cflags libjdcloudsigner` $^

main: main.o
	g++ -o $@ $^ `pkg-config --libs libjdcloudsigner`

main_static: main.o
	g++ -static -Bstatic -o $@ $^ `pkg-config --libs --static libjdcloudsigner`	
