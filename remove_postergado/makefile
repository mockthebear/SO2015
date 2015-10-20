all: remove_postergado

remove_postergado: remove_postergado.o comunication.o
	g++ remove_postergado.o comunication.o -o remove_postergado

remove_postergado.o: remove_postergado.cpp
	g++ -c remove_postergado.cpp

comunication.o: ../comunication.cpp
	g++ -c ../comunication.cpp

clean:
	rm *.o
