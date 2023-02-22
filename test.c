#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>


void show_fd(int pid, bool fd_state, bool filename_state, bool inode_state){
    DIR *dir;
    struct dirent *dir_entry;
    char str[40];
    struct stat info;
    sprintf(str,"/proc/%d/fd",pid);
    if ((dir = opendir(str)) == NULL){
        printf("PID:%d open fail\n",pid);
    }
    else{
        int i = 1;
        while ((dir_entry = readdir(dir)) != NULL) {
        if(i>2){
            char path[50];
            int fd = atoi(dir_entry -> d_name);
            sprintf(path,"/proc/%d/fd/%d",pid,fd);
            if(stat(path,&info) != 0){
                perror("stat() error");
            }
            else{
                char *buf = malloc(1024); 
            
                printf("%d   ", pid);
                if(fd_state){
                    printf("%d   ", fd);
                }
                if(readlink(path,buf,1024)!= -1){
                    if(filename_state){
                        printf("%s  ",buf);
                    }
                    if(inode_state){
                        printf("%ld", info.st_ino);
                    }    
                    printf("\n");
                }
                else{
                    perror("readlink()");
                }
                free(buf);
                   
            }
        }
 
        i++;
    }
    closedir(dir);

    }
    

}

void find_proce(bool fd_state, bool filename_state, bool inode_state){
    char uid[16];
    sprintf(uid, "%d", getuid()); // Get the user ID as a string
    
    DIR *dir;
    struct dirent *ent;
    char buf[512];
    char *endptr;
    long pid;

    // Open the /proc directory
    if ((dir = opendir("/proc")) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Read each directory in /proc
    while ((ent = readdir(dir)) != NULL) {
        // Check if the directory name is a number (i.e. a PID)
        pid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }
       
    // Check if the process owner is the current user
        sprintf(buf, "/proc/%ld/status", pid);
        FILE *fp = fopen(buf, "r");
        if (fp == NULL) {
            continue;
        }
        char line[256];
        
        while (fgets(line, sizeof(line), fp)) {
             
            if (strncmp(line, "Uid:", 4) == 0) {
                int uid_int;
                sscanf(line,"Uid:\t%d",&uid_int);
               
                if (atoi(uid)==uid_int) {

                    show_fd(pid,fd_state,filename_state,inode_state);
                }
            }
          
        }
        fclose(fp);
    }

    closedir(dir);
}



int main(int argc, char *argv[]){


    if (argc == 1) // if user enter 0 command line arguments
   {
        // show defalut composite table
        printf("PID     FD    Filename                         Inode\n");
        printf("=====================================================\n");
        find_proce(true,true,true);
        printf("=====================================================\n");
   }
   else
   {
      // scan all entered arguments
      for (int i = 1; i < argc; i++)
      {
         // if valid arguments enterd, activiate corresponding state
         if (strcmp(argv[i], "--per-process") == 0)
         {
            printf("PID     FD    \n");
            printf("===============\n");
            find_proce(true,false,false);
            printf("===============\n");
         }
         else if (strcmp(argv[i], "--systemWide") == 0)
          {
            printf("PID     FD    Filename                         \n");
            printf("===============================================\n");
            find_proce(true,true,false);
            printf("===============================================\n");
         }
         else if (strcmp(argv[i], "--Vnodes") == 0)
         {
            printf("PID      Inode \n");
            printf("====================\n");
            find_proce(false,false,true);
            printf("====================\n");
         }
         else if (strcmp(argv[i], "--composite") == 0)
         {
            printf("PID     FD    Filename                         Inode\n");
            printf("=====================================================\n");
            find_proce(true,true,true);
            printf("=====================================================\n");
         }
        //  else if (sscanf(argv[i], "--threshold=%d", &sample_size) == 1 && (sample_size > 0))
        //  {
        //     printf("The current sample size is %d\n", sample_size);
        //  }
         else
         {
            perror("Invalid command line arguments\n"); // display error message for any other arguments
            exit(0);
         }
      }

    }
}
