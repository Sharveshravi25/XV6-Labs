#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"


void rename(char*path,char*name){// to append the new dir name to old path
    int len=strlen(path);
    path[len]='/';
    path[len+1]='\0';
    char*p=path;
    p=p+len+1;
    for(int i=0;i<(int)strlen(name);i++){
        (*p)=name[i];
        p++;
    }
    *p='\0';
    return;
}

int cut_name(char*name){ // to remove file name for comparision from path
    int len=strlen(name);
    int count=0;
    char*p=(name+len-1);
    while((*p)!='/'){
        count++;
        p--;
    }
    return count;
}

void find(char*path,char*file){
   int fd;
   struct stat st;
   struct dirent dir;
   char path2[512];
   strcpy(path2,path);

    if((fd = open(path2, 0)) < 0){
      printf("ls: cannot open %s\n", path2);
      exit(-1);
    }
    if(fstat(fd, &st) < 0){//to find the type
       printf("ls: cannot stat %s\n", path2);
       close(fd);
       exit(-1);
    }

    switch(st.type){//checking-- if files then comparision,,if dir then recurse it
        case T_FILE:{
             int file_index=cut_name(path2);
             if(strcmp((path2+strlen(path2)-file_index),file)==0){
                printf("%s\n",path2);
             }
             break;
        }
        case T_DIR:{
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(path2)){
                 printf("ls: path too long\n");
                 break;
            }
           while(read(fd, &dir, sizeof(dir)) == sizeof(dir)){
               if(dir.inum == 0) continue;
               if (strcmp(dir.name,".")==0 || strcmp(dir.name,"..")==0) continue;
               
               rename(path2,dir.name);
               find(path2,file);
               strcpy(path2,path);
           }
           break;
        }
    }
    close(fd);
    return;
}

int main(int argc,char*argv[]){
    if(argc!=3){
        printf("Wrong input --failed\n");
        exit(-1);
    }
    find(argv[1],argv[2]);
    exit(0);
}