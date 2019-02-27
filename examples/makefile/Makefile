all: main

main.o: main.cpp
	g++ -std=c++11 -c -o $@ `pkg-config --cflags libjdcloudsigner` $^

main: main.o
	g++ -o $@ $^ `pkg-config --libs libjdcloudsigner`
