#include <stdio.h>
#include <time.h>
int main (int argc, char * argv[])
{
    time_t rawtime;
    struct tm *info;
    char buffer[80];

    time( &rawtime );
 
    info = localtime( &rawtime );
    if (argc> 1)
        strftime(buffer,80,(char *) argv[1], info);
    else
        strftime(buffer,80,"%Y%m%d%H%M%S", info);
 
	printf("%s", buffer);
    return(0);
}