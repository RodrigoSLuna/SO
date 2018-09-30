#include "../include/Process.h"
#include <cstdio>

void foo(){
	teste(5);
}

void teste(int x){
	printf("%d\n",x);
}

int main(){

	foo();
	printf("foi\n");
}