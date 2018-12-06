#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define BSIZE 255
/*
   get stdin 
   put OutputDebugString
*/

int main(int argc, char *argv[]) { 
  char *s;
  int r=1;
  s = malloc(BSIZE);
  while (r) 
  { 	
    memset(s, 0, BSIZE); 		
	gets(s);
	r = strlen(s);	
	if (r) OutputDebugString(s);
  } 
  return(0);
}