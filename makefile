all: go

go: test.o comunication.o
	g++ test.o comunication.o -o go

test.o: test.cpp
	g++ -c test.cpp

comunication.o: comunication.cpp
	g++ -c comunication.cpp

clean:
	rm *.o