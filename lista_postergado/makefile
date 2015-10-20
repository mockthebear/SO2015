all: lista_postergado

lista_postergado: lista_postergado.o comunication.o
	g++ lista_postergado.o comunication.o -o lista_postergado

lista_postergado.o: lista_postergado.cpp
	g++ -c lista_postergado.cpp

comunication.o: ../comunication.cpp
	g++ -c ../comunication.cpp

clean:
	rm *.o
