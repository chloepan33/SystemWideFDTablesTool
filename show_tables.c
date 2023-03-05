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

/**
 * @brief Given a pid, displaying information related to that specific pid.
 *
 * @param pid Target pid number
 * @param fd_state indication to show fd number or not
 * @param pid_state indication to show pid number or not
 * @param filename_state indication to show filename or not
 * @param inode_state indication to show inode or not
 */

void show_fd(int pid, bool fd_state, bool pid_state, bool filename_state, bool inode_state)
{
    DIR *dir;
    struct dirent *dir_entry;
    char str[40];
    struct stat info;
    sprintf(str, "/proc/%d/fd", pid); // open direcrtory for target PID
    if ((dir = opendir(str)) == NULL)
    {
        printf("PID:%d open fail\n", pid);
    }
    else
    {
        int i = 1;
        while ((dir_entry = readdir(dir)) != NULL) // loop all fd subdirectories
        {
            if (i > 2) // skip first two fd subdirectory
            {
                char path[50];
                int fd = atoi(dir_entry->d_name);
                sprintf(path, "/proc/%d/fd/%d", pid, fd);
                if (stat(path, &info) != 0)
                {
                    perror("stat() error");
                }
                else
                {
                    // show related fd infomations based on state informations
                    char *buf = malloc(1024);
                    if (pid_state)
                    {
                        printf("%d   ", pid);
                    }
                    if (fd_state)
                    {
                        printf("%d   ", fd);
                    }
                    if (readlink(path, buf, 1024) != -1)
                    {
                        if (filename_state)
                        {
                            printf("%s  ", buf);
                        }
                        if (inode_state)
                        {
                            printf("%ld", info.st_ino);
                        }
                        printf("\n");
                    }
                    else
                    {
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

/**
 * @brief Given target pid and seted threshold, print the pid number and its FD numbers if FD number is larger than threshold
 *
 * @param pid target pid
 * @param threshold thershold
 */
void show_threshold(int pid, int threshold)
{
    DIR *dir;
    struct dirent *dir_entry;
    char str[40];
    sprintf(str, "/proc/%d/fd", pid);
    if ((dir = opendir(str)) == NULL) // open target pid directory
    {
        printf("PID:%d open fail\n", pid);
    }
    else
    {
        int count = 0;
        while ((dir_entry = readdir(dir)) != NULL) // loop all FDs and count
        {
            count++;
        }
        if (count - 2 > threshold)
        {
            printf("%d (%d), ", pid, count - 2);
        }
        closedir(dir);
    }
}

/**
 * @brief find loop through all pid owned by current user and show FDs information for each pid
 *
 * @param threshold threshold
 * @param threshold_state indication to display threshold information or not
 * @param fd_state indication to show fd number or not
 * @param pid_state indication to show pid number or not
 * @param filename_state indication to show filename or not
 * @param inode_state indication to show inode or not
 */
void find_proce(int threshold, bool threshold_state, bool fd_state, bool pid_state, bool filename_state, bool inode_state)
{
    char uid[16];
    sprintf(uid, "%d", getuid()); // Get the user ID as a string

    DIR *dir;
    struct dirent *ent;
    char buf[512];
    char *endptr;
    long pid;

    // Open the /proc directory
    if ((dir = opendir("/proc")) == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Read each directory in /proc
    while ((ent = readdir(dir)) != NULL)
    {
        // Check if the directory name is a number (i.e. a PID)
        pid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0')
        {
            continue;
        }

        // Check if the process owner is the current user
        sprintf(buf, "/proc/%ld/status", pid);
        FILE *fp = fopen(buf, "r");
        if (fp == NULL)
        {
            continue;
        }
        char line[256];

        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "Uid:", 4) == 0)
            {
                int uid_int;
                sscanf(line, "Uid:\t%d", &uid_int); // get user id for the current directory

                if (atoi(uid) == uid_int) // compare directory's uid with current user id
                {
                    // show information only the directory is owned by current user
                    if (threshold_state)
                    {
                        show_threshold(pid, threshold);
                    }
                    else
                    {
                        show_fd(pid, fd_state, pid_state, filename_state, inode_state);
                    }
                }
            }
        }
        fclose(fp);
    }

    closedir(dir);
}

/**
 * @brief given flagged that indicate to show which tables, print headers for each table and set correspoding information state
 *        if a specific pid is given, only display that pid's info
 *
 * @param composite_state indication to print composite table
 * @param process_state indication to print process table
 * @param systemWide_state indication to print systemWide table
 * @param vnodes_state indication to print vnode table
 * @param pid targeted pid, equals to -1 if want to read through all avaliable pids
 */
void show_tables(bool composite_state, bool process_state, bool systemWide_state, bool vnodes_state, int pid)
{
    int threshold = -1;

    if (composite_state)
    {
        // show composite table
        printf("PID     FD    Filename                         Inode\n");
        printf("=====================================================\n");
        if (pid == -1)
        {
            find_proce(threshold, false, true, true, true, true);
        }
        else
        {
            show_fd(pid, true, true, true, true);
        }
        printf("=====================================================\n");
    }
    if (process_state)
    {
        // show process table
        printf("PID     FD    \n");
        printf("===============\n");
        if (pid == -1)
        {
            find_proce(threshold, false, true, true, false, false);
        }
        else
        {
            show_fd(pid, true, true, false, false);
        }
        printf("===============\n");
    }
    if (systemWide_state)
    {
        // show systemWide table
        printf("PID     FD    Filename                         \n");
        printf("===============================================\n");
        if (pid == -1)
        {
            find_proce(threshold, false, true, true, true, false);
        }
        else
        {
            show_fd(pid, true, true, true, false);
        }
        printf("===============================================\n");
    }
    if (vnodes_state)
    {
        // show vnodes table
        printf("PID      Inode \n");
        printf("====================\n");
        if (pid == -1)
        {
            find_proce(threshold, false, false, true, false, true);
        }
        else
        {
            show_fd(pid, false, true, false, true);
        }
        printf("====================\n");
    }
}

/**
 * @brief save composite table to a file, either in txt or binary format
 *
 * @param filename target file name
 * @param mode indicate to write in TXT form or binary form
 */
void save_output(char *filename, char *mode)
{
    FILE *output = fopen(filename, mode);

    char uid[16];
    sprintf(uid, "%d", getuid()); // Get the user ID as a string

    DIR *dir;
    struct dirent *ent;
    char buf[512];
    char *endptr;
    long pid;

    // Open the /proc directory
    if ((dir = opendir("/proc")) == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Read each directory in /proc
    while ((ent = readdir(dir)) != NULL)
    {
        // Check if the directory name is a number (i.e. a PID)
        pid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0')
        {
            continue;
        }

        // Check if the process owner is the current user
        sprintf(buf, "/proc/%ld/status", pid);
        FILE *fp = fopen(buf, "r");
        if (fp == NULL)
        {
            continue;
        }
        char line[256];

        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "Uid:", 4) == 0)
            {
                int uid_int;
                sscanf(line, "Uid:\t%d", &uid_int);

                if (atoi(uid) == uid_int) // check if the current user own this pid directory
                {
                    DIR *dir;
                    struct dirent *dir_entry;
                    char str[40];
                    struct stat info;
                    sprintf(str, "/proc/%ld/fd", pid);
                    if ((dir = opendir(str)) == NULL)
                    {
                        printf("PID:%ld open fail\n", pid);
                    }
                    else
                    {
                        int i = 1;
                        while ((dir_entry = readdir(dir)) != NULL) // loop all FDs
                        {
                            if (i > 2)
                            {
                                char path[50];
                                int fd = atoi(dir_entry->d_name);
                                sprintf(path, "/proc/%ld/fd/%d", pid, fd);
                                if (stat(path, &info) != 0)
                                {
                                    perror("stat() error");
                                }
                                else
                                {
                                    char *filename = malloc(512);
                                    char inf[1024];
                                    if (readlink(path, filename, 512) != -1)
                                    {
                                        // store all information in a string
                                        sprintf(inf, "%ld   %d   %s   %ld\n", pid, fd, filename, info.st_ino);
                                    }
                                    // check to see which format to write into
                                    if (strcmp(mode, "w") == 0)
                                    {
                                        fprintf(output, "%s", inf);
                                    }
                                    else
                                    {
                                        fwrite(inf, 1, sizeof(inf), output);
                                    }

                                    free(filename);
                                }
                            }

                            i++;
                        }
                        closedir(dir);
                    }
                }
            }
        }
        fclose(fp);
    }

    closedir(dir);

    fclose(output);
}

int main(int argc, char *argv[])
{
    //set default value for all states
    int threshold = -1;
    int pid = -1;

    bool composite_state = false;
    bool process_state = false;
    bool systemWide_state = false;
    bool vnodes_state = false;
    bool threshold_state = false;
    bool output_bi = false;
    bool output_txt = false;

    if (argc == 1) // if user enter 0 command line arguments
    {
        //display default composite fd table
        show_tables(true, false, false, false, pid);
    }
    else
    {
        // scan all entered arguments
        for (int i = 1; i < argc; i++)
        {
            // if valid arguments enterd, activiate corresponding state
            if (strcmp(argv[i], "--per-process") == 0)
            {
                process_state = true;
            }
            else if (strcmp(argv[i], "--systemWide") == 0)
            {
                systemWide_state = true;
            }
            else if (strcmp(argv[i], "--Vnodes") == 0)
            {
                vnodes_state = true;
            }
            else if (strcmp(argv[i], "--composite") == 0)
            {
                composite_state = true;
            }
            else if (sscanf(argv[i], "--threshold=%d", &threshold) == 1)
            {
                if (threshold < 0)
                {
                    printf("Invalid input");
                }
                else
                {
                    threshold_state = true;
                }
            }
            else if (sscanf(argv[i], "%d", &pid) == 1)
            {
                if (pid < 0)
                {
                    printf("User input PID invalid\n");
                }
            }
            else if (strcmp(argv[i], "--output_TXT") == 0)
            {
                output_txt = true;
            }
            else if (strcmp(argv[i], "--output_binary") == 0)
            {
                output_bi = true;
            }
            else
            {
                perror("Invalid command line arguments\n"); // display error message for any other arguments
                exit(0);
            }
        }
    }

    // if only enterd one positional argument, activate composite table state
    if (pid != -1 && argc == 2)
    {
        composite_state = true;
    }

    //displaying tables
    show_tables(composite_state, process_state, systemWide_state, vnodes_state, pid);

    if (threshold_state)
    {
        printf("## Offending processes:\n");
        find_proce(threshold, true, false, false, false, false);
        printf("\n");
    }

    //save output based on user input flags
    if (output_txt)
    {
        save_output("compositeTable.txt", "w");
    }
    if (output_bi)
    {
        save_output("compositeTable.bin", "wb");
    }
}
