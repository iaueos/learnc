#include "stdio.h"
#include "string.h"
#include "stddef.h"

typedef unsigned long long NU;
const char * NUM_FMT ="%llu";

const char * help="\
\n\
testnum [bit|pow|max] num1 num2\n\
\n\
";


void say(NU l)
{
    printf(NUM_FMT, l);
    printf("\r\n");
}

NU tonum(char *instr)
{
  NU retval;
  int i;

  retval = 0;
  for (; *instr; instr++) {
    retval = ((*instr)>='0'&&(*instr)<='9')
           ? (10*retval + (NU) (*instr - '0'))
           : retval;
  }
  return retval;
}

int isnum(char *c)
{
    int r = 1;
    for(;*c && r;c++) {
        r = (*c>='0')&&(*c<='9');
    }
    return r;
}


NU power2min1(NU n)
{
    NU r = 1;
    for(NU i =1; (i <= n) && (n <= 64); i++)
        r = (r << 1);
    r = r -1;
    return r;
}

NU power2(NU n)
{
    NU r = 1;
    for(NU i =1; (i <= n) && (n <= 64); i++)
        r = (r << 1);
    return r;
}

NU maxval() {
    NU r= ~ ((NU)0);
    if (r<0)
        r=~((NU) 1 << ((sizeof(r) *8)-1));
    return (r);
}


int main(int argc, char *argv[])
{
    NU i, x, y, z;
    unsigned char a = 0;
    int act = 0;
    char * filename;
    char reads;

    char * argi;
    char s[255];

    x=0;
    y=0;
    i=0;

    if (argc <= 1) {
        act = 9;
    }

    for (a=1; a<argc; a++) {
        if (isnum(argv[a]))
        {
            if ((x==0)) {
                x = tonum(argv[a]);
            } else if ((y==0)) {
                y = tonum(argv[a]);
            }
        }
        else
        {
            argi = argv[a];
            if (strcmp(argi, "?")==0)
                act = 9;
            else if(strcmpi(argi, "bit")==0)
                act = 6;
            else if(strcmpi(argi, "pow")==0)
                act = 5;
            else if(strcmpi(argi, "max")==0)
                act = 4;
        }
    }

    if (act == 9)
    {
        printf(help);
        return(1);
    }

    if (act==6)
    {
        if (y < x) y = 64;

        for (i=x; i>=0 && i<=y && i<=64; i++)
            say(power2min1(i));
        return (0);
    }


    if (act==5)
    {
        if (y < x) y = 64;

        for (i=x; i>=0 && i<=y && i<=64; i++)
            say(power2(i));
        return (0);
    }

    if (act==4) {
        say(maxval());
        return(0);
    }
}