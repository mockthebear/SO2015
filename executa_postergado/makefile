all: executa_postergado

executa_postergado: executa_postergado.o comunication.o
	g++ executa_postergado.o comunication.o -o executa_postergado

executa_postergado.o: executa_postergado.cpp
	g++ -c executa_postergado.cpp

comunication.o: ../comunication.cpp
	g++ -c ../comunication.cpp

clean:
	rm *.o
