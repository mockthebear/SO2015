#include <sys/time.h>
#include <iostream>
#include <unistd.h>

// programa teste

int main(){
	for (int i=0;i<10;i++){
    	std::cout << "oi\n";
    	sleep(1);
 	}
 	return 0;
}
