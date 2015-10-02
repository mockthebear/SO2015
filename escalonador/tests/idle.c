#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main(){
    struct timeval bgn, end;
    gettimeofday(&bgn, NULL);
    int n = 0;
    while (n<10){
        gettimeofday(&end, NULL);
        unsigned int seconds  = end.tv_sec  - bgn.tv_sec;
        unsigned int useconds = end.tv_usec - bgn.tv_usec;
        unsigned int mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        if (mtime >= 500){
            n++;
            printf("N is = %d\n",n);
            gettimeofday(&bgn, NULL);
        }
    }

    return 0;
}
