all: servidor

servidor: servidor.o comunication.o task.o
	g++ task.o servidor.o comunication.o -o servidor -pthread

task.o: task.cpp
	g++ -c task.cpp -std=gnu++0x

servidor.o: servidor.cpp
	g++ -c servidor.cpp -std=gnu++0x

comunication.o: ../comunication.cpp
	g++ -c ../comunication.cpp -std=gnu++0x

clean:
	rm *.o
