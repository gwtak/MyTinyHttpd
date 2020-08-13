server.out:MyTinyHttpd.cpp
	g++ MyTinyHttpd.cpp -o server.out -pthread
	
.PHONY:clean
	rm -rf *.o