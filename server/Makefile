all: build

build: main.cpp
	mkdir -p out
	g++ -o out/server -std=c++11 -I. main.cpp TcpServer.cpp RTree.cpp

exec: build
	./out/server

clean:
	rm -rf out
