#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

void show_fd(int pid){
    DIR *dir;
    struct dirent *dir_entry;
    char str[30];
    struct stat info;
    sprintf(str,"/proc/%d/fd",pid);
    if ((dir = opendir(str)) == NULL){
        perror("opendir");
    }
    int i = 1;
    while ((dir_entry = readdir(dir)) != NULL) {

        if(i>2){

            if(fstat(atoi(dir_entry -> d_name),&info) != 0){
                perror("fstat() error");
            }
            else{
                printf("PIN:%s  Inode:%ld\n",dir_entry -> d_name,info.st_ino);
            }
        }
 
        i++;
    }
    closedir(dir);

}

int main(int argc, char *argv[]){
    show_fd(3281874);

}
