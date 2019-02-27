all: main main_static

main.o: main.cpp
	g++ -std=c++11 -c -o $@ `pkg-config --cflags libjdcloudsigner` $^

main: main.o
	g++ -o $@ $^ `pkg-config --libs libjdcloudsigner`

main_static: main.o
	@if test `uname` = Darwin; then \
		touch $@; \
	else \
		g++ -static -Bstatic -o $@ $^ `pkg-config --libs --static libjdcloudsigner`; \
	fi
