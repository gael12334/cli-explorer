Copyright (c) 2026 Gaël Fortier

## Disclaimer
This project is unstable and likely to crash. 
This project is full of bugs and unexpected behaviors.
This project is meant as an artefact for my portfolio.
Avoid using this program.

## CLI file explorer
Essentially, this is the equivalent of the `cd` and `ls` commands in Linux. It requires SDL2 to run as it is used for opening and rendering images in a window. 
```
Directory: /
1     DT_DIR     sys
2     DT_DIR     etc
3     DT_LNK     libx32
4     DT_DIR     dev
5     DT_DIR     usr
6     DT_LNK     lib
7     DT_DIR     root
8     DT_DIR     cdrom
9     DT_DIR     mnt
10    DT_DIR     sbin.usr-is-merged
11    DT_LNK     bin
12    DT_DIR     boot
13    DT_DIR     proc
14    DT_DIR     opt
15    DT_DIR     home
16    DT_LNK     lib64
17    DT_DIR     tmp
18    DT_DIR     run
19    DT_DIR     srv
20    DT_DIR     ..
21    DT_DIR     bin.usr-is-merged
22    DT_LNK     lib32
23    DT_DIR     lib.usr-is-merged
24    DT_DIR     lost+found
25    DT_DIR     timeshift
26    DT_LNK     sbin
27    DT_DIR     media
28    DT_DIR     var
29    DT_DIR     .
30    DT_DIR     snap
command > 
```
There are 3 commands available. When entering the command, only type the command name since if there are more parameters, the program will prompt you for each individual parameters. 

- `set`: opens the directory specified by an absolute path. A second prompt will ask you to specify the absolute path.
```
command > set 
absolute path > /home/gael/Bureau/demo
Directory: /home/gael/Bureau/demo
1     DT_DIR     folder1
2     DT_REG     file1.txt
3     DT_DIR     ..
4     DT_DIR     .
command > 

```
- `cd`: opens a child directory. A second prompt will ask you for the number of the entry associated with the desired child directory.
```
command > cd
directory # > 15
Directory: /home
1     DT_DIR     gael
2     DT_DIR     ..
3     DT_DIR     .
command > 
```
- `N`: For N a number between 1 and X, and X being the number of available entries in the directory, the program will attempt to load the Nth entry in the directory as an image and, if successful, open a window displaying the selected image. To close the window, select the window and press on any key.

![](img.png)

- `quit`: closes the program.
