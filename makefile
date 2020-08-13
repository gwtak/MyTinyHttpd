server.out:TinyWebServer.cpp
	g++ TinyWebServer.cpp -o server.out -pthread
	
.PHONY:clean
	rm -rf *.o