//Exercicio de Pthreads de programação paralela
//Usado no escalonador de Sistemas operacionais.
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <sys/time.h>
#include <fstream>
int threads=4;
int seed = -1;
long int tosses = 10000;
pthread_t *threads_;

struct timeval start, end;

typedef struct myParameter_{
    int inside;
    int toss;
    float ret;
    int mythread;
    unsigned int seed;
} myParameter;

myParameter *params;

void *genPI(void *param)
{
       myParameter *P = (myParameter*)param;

       P->inside = 0;
        for (int i=0;i<P->toss;i++){
            float Pointx = (rand_r(&P->seed)%10000000)/5000000.0f  -1;
            float Pointy = (rand_r(&P->seed)%10000000)/5000000.0f  -1;
            float sq = Pointx*Pointx + Pointy*Pointy;
            if (sq <= 1)
               P->inside++;
        }

       pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
    if (argc <= 1){
        printf("Use parameters:-t=10  to define teh usege of 10 threads\n-s=1337 to define the seed\n-c=10000000 the amount of points\n");
        return 0;
	}
	for (int i=0;i<argc;i++){
        if (argv[i][1] == 't'){
            sscanf(argv[i],"-t=%d",&threads);
        }
        if (argv[i][1] == 's'){
            sscanf(argv[i],"-s=%d",&seed);
        }
        if (argv[i][1] == 'c'){
            sscanf(argv[i],"-c=%ld",&tosses);
        }
	}
	gettimeofday(&start, NULL);
	threads_ = new  pthread_t[threads];
	params = new myParameter[threads];
	float *ret =new float[threads];
	if (seed == -1){
        seed = time(NULL);
	}
    for (int i = 0; i < threads; i++)
    {
        params[i].seed = seed + i;
        params[i].toss = tosses/(float)threads;
        params[i].mythread = i;
        pthread_create(&threads_[i], NULL, genPI, (void *) &params[i]);
    }
    for (int i = 0; i < threads; i++)
    {
        pthread_join(threads_[i], NULL);
    }
    int N = 0;

    for (int i = 0; i < threads; i++)
    {
        N += params[i].inside;
    }
    gettimeofday(&end, NULL);




    std::fstream file("static.txt", std::ios::out | std::ios::app);
    //
    double dur = (end.tv_sec - start.tv_sec) + ( (end.tv_usec - start.tv_usec )/100000000.0);

    std::cout << "Value of pi is around: " << ((double)N/(float)tosses)*4.0 << "Finished in " << dur <<"\n";
    file << "[" << seed << "," << tosses << "," << threads << ","  << ((double)N/(float)tosses)*4.0 << "=" << (double)dur << "]" << std::endl;
    //
    file.close();

    return 1;
}
