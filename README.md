# **System-Wide FD Tables**

---

## ****Getting Started****

---

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### **Prerequisites**

You need a C compiler to build the program. GCC is recommended for this purpose. The program should be run on a Linux operating system. 

### **Installing**

1. Clone the repository to your local machine 
    
    ```bash
    git clone https://github.com/Chloepan33/System-Wide-FD-Tables-Tool.git
    ```
    
2. Compile the program
    
    ```bash
    gcc show_tables.c -o show_tables
    ```
    
3. Run the program
    
    ```bash
    ./show_table
    ```
    

## Usage

Run the program directly 

```bash
./show_table
```

The program will display table in composite view.

```bash
PID     FD    Filename                         Inode
=====================================================
3512504   0   /dev/null  6
3512504   1   socket:[2300093433]  2300093433
3512504   2   socket:[2300093433]  2300093433
3512504   3   socket:[2300094480]  2300094480
3512504   4   anon_inode:[eventpoll]  10286
3512504   5   anon_inode:[signalfd]]  10286
3512504   6   anon_inode:inotifyfd]]  10286
3512504   7   /sys/fs/cgroup/unified/user.slice/user-23224130.slice/user@23224130.service  258061
3512504   8   anon_inode:[timerfd]ed/user.slice/user-23224130.slice/user@23224130.service  10286
3512504   9   anon_inode:[eventpoll]/user.slice/user-23224130.slice/user@23224130.service  10286
3512504   10   /proc/3512504/mountinfouser.slice/user-23224130.slice/user@23224130.service  2300094488
3512504   11   anon_inode:inotifytinfouser.slice/user-23224130.slice/user@23224130.service  10286
3512504   12   socket:[2300094499]infouser.slice/user-23224130.slice/user@23224130.service  2300094499
3512504   13   anon_inode:inotify]infouser.slice/user-23224130.slice/user@23224130.service  10286
3512504   14   /proc/swaps  4026532071
3512504   15   socket:[2300094489]infouser.slice/user-23224130.slice/user@23224130.service  2300094489
3512504   16   socket:[2300094491]infouser.slice/user-23224130.slice/user@23224130.service  2300094491
3512504   17   socket:[2300094492]infouser.slice/user-23224130.slice/user@23224130.service  2300094492
3512504   18   socket:[2300094493]infouser.slice/user-23224130.slice/user@23224130.service  2300094493
3512504   19   socket:[2300094494]infouser.slice/user-23224130.slice/user@23224130.service  2300094494
3512504   20   socket:[2300094495]infouser.slice/user-23224130.slice/user@23224130.service  2300094495
3512504   21   anon_inode:[timerfd]nfouser.slice/user-23224130.slice/user@23224130.service  10286
3512504   26   socket:[2300094529]]nfouser.slice/user-23224130.slice/user@23224130.service  2300094529
3512504   27   socket:[2300094530]]nfouser.slice/user-23224130.slice/user@23224130.service  2300094530
3512504   28   socket:[2300094531]]nfouser.slice/user-23224130.slice/user@23224130.service  2300094531
3512504   29   socket:[2300094532]]nfouser.slice/user-23224130.slice/user@23224130.service  2300094532
3512504   30   socket:[2300094533]]nfouser.slice/user-23224130.slice/user@23224130.service  2300094533
3512504   31   socket:[2300094534]]nfouser.slice/user-23224130.slice/user@23224130.service  2300094534
```

The following command line arguments are available for use and can be **combined**.

- **`--per-process`**: Display the process FD table.
- **`--systemWide`**: Display the system-wide FD table.
- **`--Vnodes`**: Display the Vnodes FD table.
- **`--composite`**: Display the composed view of all three tables.
- **`--threshold=X`**: Flag processes with more than X FDs assigned.
- **`--output_TXT`**: Save composite table in text (ASCII) into a file named compositeTable.txt.
- **`--output_binary`**: Save composite table in binary format into a file named compositeTable.bin.

The program also takes propositional arguments indicating a particular process ID number (PID). 

If no PID is specified, the program will attempt to process all the currently running processes for the user executing the program.

### How do I solve

I started by researching the structure and content of the `/proc` file system. I focused specifically on the files related to file descriptors, such as `/proc/[pid]/fd.` I then implemented code to read this information from these files and to format it into tables.

To handle the various command line options and arguments, I implemented command line argument parsing techniques. This allowed users to specify which tables they wanted to see, and also to set a threshold value for the number of file descriptors a process can have before it is flagged in the output. 

Additionally, I made sure to handle edge cases, such as when no arguments were provided, file fail to open or when a specified process ID did not exist. 

Finally, I tested the program to ensure that it worked as expected under different scenarios.
