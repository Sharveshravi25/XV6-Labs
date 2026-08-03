#include "kernel/types.h"
#include "user/user.h"
void prime(int*read_pipe){
    int num;
    if(read(read_pipe[0],&num,4)==0){// to read 1st num, if no number then exit to stop further recurrsion
        exit(0);
    }
    printf("prime %d\n",num);

    int p2[2]; //pipe to send only the numbers not divisible by the parent's base_num
    if(pipe(p2)<0){
        printf("pipe2 failed\n");
        exit(-1);
    } 
    int pid2=fork(); 
    if(pid2<0){ 
        printf("fork failed\n");
        exit(-1);
    }
    else if(pid2==0){ 
        close(p2[1]); 
        close(read_pipe[0]);
        prime(p2); 
        exit(0);
    } 
    else{ 
        int base_num=num; 
        close(p2[0]); 
        while(read(read_pipe[0],&num,4)>0){            
            if(num%(base_num)!=0){ 
                write(p2[1],&num,4); 
            }
         }
        close(read_pipe[0]);
        close(p2[1]);
        int status; 
        wait(&status); 
        exit(0);
    } 
    exit(0);
}
int main(){ 
    int p[2]; 
    if(pipe(p)<0){
        printf("pipe failed\n"); 
        exit(-1);
    } 
    int pid=fork(); //1st fork,,which seeds the numbers from 2-35
    if(pid<0){ 
        printf("fork failed\n");
        exit(-1); 
    } 
    else if(pid==0){
        close(p[1]); 
        prime(p);
    }
    else{
        close(p[0]);
        int i=2;
        while(i<36){
            write(p[1],&i,4); i++;
        } 
        close(p[1]);
        }
        int status; 
        wait(&status); 
        exit(0);
}