#include "kernel/types.h"  
#include "user/user.h" 

int main(int argc, char *argv[]){
   int p1[2];
   int p2[2];
   if(pipe(p1)<0 ||pipe(p2)<0){
      printf("pipe failed\n");
      exit(1);
   }
   int fd=fork();//forking
   if(fd<0){
      printf("fork failed\n");
      exit(1);
   }
   if(fd==0){ //child process
      close(p1[1]);
      close(p2[0]);
      char buf2[2]="";
      if(read(p1[0],buf2,1)==1){
          printf("%d: received ping\n",getpid());
      }
      write(p2[1],buf2,1);
      close(p1[0]); 
      close(p2[1]);
      exit(0);
   }
   else{ //parent process
      close(p1[0]);
      close(p2[1]);
      char buf[2]="1"; 
      char buf1[2]="";
      write(p1[1],buf,1);  
      if(read(p2[0],buf1,1)==1){
          printf("%d: received pong\n",getpid());
         }     
      close(p1[1]); 
      close(p2[0]);
      exit(0);
      
   }
     
   exit(0);
}