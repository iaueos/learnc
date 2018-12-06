#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <sqlite3.h>

void Help() {
   printf("asq - ask query to sqlite database\n\n" 
    "asq <db> \"query\" [<linebreak> [<separator>]]\n\n"
    "<db> : sqlite database, must exist\n"     
   "<linebreak> separate each row with linebreak\n"
   "<separator> alternate column separator other than tab");   
   exit(1);
}

int main(int argc, char **argv){
  sqlite3 *db=NULL;
  sqlite3_stmt *p;
  int rc = 0;
  int lb = 0;
  int i, j;
  char *sep = "\t";
  char *linebreak = "\r\n";
  do { 
      if(argc<3) Help();
      lb = (argc > 3);
      if (lb) { 
          if (strcmp(argv[3], ".") != 0) {
            strcpy(linebreak,argv[3]);
          }
      }
      if (argc > 4)
         strcpy(sep, argv[4]);
         
      if (access(argv[1], R_OK) == -1) { 
        fprintf(stderr, "File not found\n");
        exit(1);
      }
        
      rc = sqlite3_open(argv[1], &db);
      if (rc) 
        break;
      rc = sqlite3_prepare(db, argv[2], strlen(argv[2]), &p, NULL); 
      if (rc) 
        break;
      j = -1;
      setmode(STDOUT_FILENO, _O_BINARY);
      while (rc = sqlite3_step(p) == SQLITE_ROW) { 
        if (j < 0) j = sqlite3_column_count(p);
        for (i=0; i<j; i++) { 
            if (i > 0) 
               printf(sep);
            printf(sqlite3_column_text(p, i));   
        } 
        if (lb) 
            printf(linebreak);
      } 
      sqlite3_finalize(p);      
  }
  while(0);
  if (rc) 
    fprintf(stderr, 
     "SQL error (%d) %s %s\n", sqlite3_errcode(db), sqlite3_errmsg(db));        
  
  if (db!=NULL) 
    sqlite3_close(db);  
  return 0;
}
