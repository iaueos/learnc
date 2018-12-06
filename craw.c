#include <stdio.h>
#include <stdlib.h>

#define SZ 4096
 
void ShowHelp() {
    printf("craw <input> <output> [<start> [<length>]]\n"
    "cut raw files <input> write to <output>\n"
    "start=9\n");
} 
 
int main(int argc, char *argv[])
{
    char buffer[SZ];
    
    int i;
    int rstart = 9;
    int rlength=-1;
    int fsize = 0;

    if (argc < 2) { 
        ShowHelp();
        return EXIT_FAILURE;
    }
    /*
    printf(argv[1]);
    printf("\n");
    printf(argv[2]);
    printf("\n");
    */
    if (argc > 3) 
        rstart = atoi(argv[3]);
    if (argc > 4)
        rlength = atoi(argv[4]);
    // printf("start=%d length=%d\n", rstart, rlength);
    
    FILE *fp = fopen(argv[1], "r+b");
    // fseek(fp, 0, SEEK_END); 
    // fsize = ftell(fp);
    // rewind(fp);
    FILE *fw = fopen(argv[2], "w+b"); 
    if ((fp == NULL) || (fw == NULL)) {
        perror("Failed to open files");
        return EXIT_FAILURE;
    }
    int seekr = fseek(fp, rstart-1, SEEK_SET);
    if (seekr > 0) { 
        return EXIT_FAILURE;
    }
    
    int r=0;
    int p=0;
    do {
        int rz = SZ;
        p = ftell(fp);
        // printf("pos=%d\n", p);
        if ((rlength < 0) || (rlength > SZ)) 
            rz = SZ;
        else
            rz = rlength;
        /*if ((fsize - p) < rz)
            rz = fsize - p;*/
        // printf("read %d\n",rz);
        memset(buffer, 0, SZ);
        r = fread(buffer, 1, rz, fp);
        if (rlength>0)
            rlength -= r;
        // printf("read %d length %d\n", r, rlength);
        if (r > 0) { 
            fwrite (buffer, r, 1, fw);             
        }
    } while ((r > 0) && (rlength!=0));
    
    fclose(fp);
    fclose(fw);
    
    return EXIT_SUCCESS;
}