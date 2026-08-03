#include "kernel/types.h"
#include "user/user.h"  
#include "kernel/param.h"


char * token(char*arg){//to return ptr where \n is there to know where to parse
    char*buf=arg;
    while((*buf)!='\n'){
       if(*buf=='\0') return 0;
        buf++;
    }
    return buf;
}
void xargs(char*argument,int argc1,char*argv1[]){
     char*token_ptr=token(argument);//parsing str,to get arg before \n
     if(!token_ptr){
        exit(0);
     }
     int len=token_ptr-argument;
     char*token_arg=(char*)malloc(len);
     memmove(token_arg,argument,len);
     token_arg[len]='\0';

     // forking to exec
     int pid=fork();
     if(pid<0){
        printf("forked failed\n");
      }
     else if(pid==0){//child-- to exec the newly passed token along with xargs arg
            char*exec_argv[MAXARG];
            int i=1;
            while(i<argc1){
                exec_argv[i-1]=argv1[i];
                i++;
            }
            exec_argv[i-1]=token_arg;
            exec_argv[i]='\0';
            exec(*(exec_argv+0),exec_argv);
     }
     else{//parent-to call next iteration of string parsing and exec
        token_ptr+=1;
        free(token_arg);
        int status;
        wait(&status);
        xargs(token_ptr,argc1,argv1);        
     }     
     exit(0);

}

int main(int argc,char*argv[]){
    char c;
    char argument[512];
    int index=0;
    while(read(0,&c,1)>0){//reading char by char and then appending to array
        if(argument[index-1]=='\\'&& c=='n'){
               printf("True:%d\n",index);
               argument[index-1]='\n';
        }
        else{
          argument[index]=c;
          index++;
        }
    }
    argument[index]='\n';//ending with \n to avoid edge case while parsing
    argument[index+1]='\0';
    xargs(argument,argc,argv);

    exit(0);
}