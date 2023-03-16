ipkcpc: ipkcpc.o
	g++ ipkcpc.o -o ipkcpc

ipkcpc.o: ipkcpc.cpp
	g++ -c ipkcpc.cpp

clean:
	rm *.o ipkcpc
