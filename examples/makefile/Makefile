all: main main_static

main.o: main.cpp
	g++ -std=c++11 -c -o $@ `pkg-config --cflags libjdcloud_signer` $^

main: main.o
	g++ -o $@ $^ `pkg-config --libs libjdcloud_signer`

main_static: main.o
	@if test `uname` = Darwin; then \
		touch $@; \
	else \
		g++ -static -Bstatic -o $@ $^ `pkg-config --libs --static libjdcloud_signer`; \
	fi
