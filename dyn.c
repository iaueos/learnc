// DYnamic array of structures

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node {
 char *str;
 int len;
};

int main(void) {
 struct node **strarray = NULL;
 int i = 0, count = 0;
 char line[1024];

 while(fgets(line, 1024, stdin) != NULL) {
  /* add ONE element to the array */
  strarray = (struct node **)realloc(strarray, (count + 1) * sizeof(struct node *));

  /* allocate memory for one `struct node` */
  strarray[count] = (struct node *)malloc(sizeof(struct node));

  /* copy the data into the new element (structure) */
  strarray[count]->str = strdup(line);
  strarray[count]->len = strlen(line);
  count++;
 }

 /* print the array */
 for(i = 0; i < count; i++) {
  printf("--\n");
  printf("[%d]->str: %s", i, strarray[i]->str);
  printf("[%d]->len: %d\n", i, strarray[i]->len);
 }

 /* free all strarray elements */
 for(i = 0; i < count; i++) {
  free(strarray[i]->str);
  free(strarray[i]);
  i++;
 }
 free(strarray);
  
 return 0;
}