#include <unistd.h>
#include <stdio.h>
int main(void){
 int retorno,i;
 for (i=0;i<4;i++){
 retorno=fork();
 if (retorno==0){
 printf("\nSou o filho No.:%d\n",i);
 i=4;
 }
 } return 0;
}

