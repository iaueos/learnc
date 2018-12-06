#include "stdio.h"
#include "io.h"
#include "stdbool.h"

int main(int argc, char *argv[]) {
	char * filename;
	FILE * f;
	
	if (argc > 1) {
		filename = argv[1];
		
		if (access(filename, R_OK) != -1) {
			// 
		} else {
			return 2;  // file not exits 
		}
		
		f = fopen(filename, "rb");
		
	} else
		return 1;

	int all = 0;
		
	if (argc > 2) {
		if (stricmp(argv[2], "a",1) ==0)
			all = 1;
	
		if (stricmp(argv[2], "q",1) ==0)
			all = 2;
			
		if (stricmp(argv[2], "qa",1) ==0 || stricmp(argv[2], "aq",1) ==0)
			all = 3;
	}
	long long q,q1;
	long l, l1;

	int z;
	z = ((all & 2) >0) ? sizeof(long long): sizeof(long);
	if ((all >= 0) && ((all & 1) < 1)) {
		fseek(f, 0, SEEK_SET); 
		
		
		if (all & 2 > 0) {			
			fread(&q, z, 1, f);
		} else {
			fread(&l, z, 1, f);
		}		
		fseek(f, -1 * z, SEEK_END); 
		if (all & 2 > 0) {
			fread(&q1, z, 1, f);
		} else {
			fread(&l1, z, 1, f);
		}
		if (all & 2 > 0)
			printf("%lld %lld\n", q, q1);
		else 
			printf("%d %d\n", l, l1);
		
	} else if ((all & 1) > 0 ) {
		fseek(f, 0, SEEK_SET); 
		bool b = true;
		if ((all & 2) > 0) {
			
			do 
			{
				fread(&q, z, 1, f);
				b = !feof(f);
				if (!b) break;
				printf("%lld\n", q);
			} while(b);
		} else {

			do 
			{
				fread(&l, z, 1, f);
				b = !feof(f);
				if (!b) break;
				printf("%d\n", l);
			} while(b);
		}
	}	
	if (all>=0)
		fclose(f);
}