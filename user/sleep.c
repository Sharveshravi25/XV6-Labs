#include "kernel/types.h"
#include "user/user.h"

int main(int argc,char*argv[]){
    if(argc!=2){
        printf("Wrong input-failed\n");
        return -1;
    }
    int time_to_sleep=atoi(argv[1]);
    sleep(time_to_sleep);
    exit(0);
}