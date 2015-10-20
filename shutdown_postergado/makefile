all: shutdown_postergado

shutdown_postergado: shutdown_postergado.o comunication.o
	g++ shutdown_postergado.o comunication.o -o shutdown_postergado

shutdown_postergado.o: shutdown_postergado.cpp
	g++ -c shutdown_postergado.cpp

comunication.o: ../comunication.cpp
	g++ -c ../comunication.cpp

clean:
	rm *.o
