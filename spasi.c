#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    FILE *fo;
    long i,j;
    if (argc < 3) { 
        printf("spasi <file> <length>\n");
        exit(0);
    } 
    fo = fopen(argv[1], "w+");
    j = atol(argv[2]);
    for (i=0; i<j; i++) {
       fputc(' ', fo);
    } 
    fclose(fo);
}    
    