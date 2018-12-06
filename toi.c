#include <stdio.h>
#include <stdlib.h>
// #include <string.h>

int main (int argc, char * argv[] ) {
   int i = 0;
   char *rx; 
   
   for (i =1; i < argc; i++) {
		long long l = strtoul(argv[i], &rx, 10);
		printf("%lld\r\n", l);
   }
   return(0);
}