#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0


void ShowHelp()
{
    printf(
        "flip file1 file2 ... fileN \n"
        "read all file(s) output per line mixed\n");
        
    exit(0);
}

int main(int argc, char *argv[])
{
    FILE * fo[64];
    int ieof[64];
    char s[4096];
    int i, j, k, fi;
    fi = 0;    
    if (argc < 2) ShowHelp();
    
    setmode(STDOUT_FILENO, _O_BINARY);
    for (i=1; i<argc; i++) { 
        fo[i] = fopen(argv[i], "r+");
        ++fi;
        if (fi > 64) break;
        
        ieof[i] = FALSE;
    } 
    j = 0; 
    k = 0;
    // prevent infinite loop, max 2^31-1 
    while (j  < argc -1 && k < 2147483647) { 
        j = 0;
        for (i=1; i<argc; i++) {
            if (!ieof[i]) { 
                if (feof(fo[i])) {
                    ieof[i] = TRUE; 
                    ++j;
                } else {
                    if (fgets(s, 4096, fo[i])) { 
                        int l;
                        ++ k;
                        for (l=0; l<4096; l++) 
                            if (s[l] != 0)
                                fputc(s[l],stdout);
                            else
                                break;                        
                    }
                    else
                        ieof[i] = TRUE;
                }
            } 
            else { 
                ++j;
            }
        }        
    } 
    for (i=1; i <argc; i++) 
        fclose(fo[i]);
}