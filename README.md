Copyright (c) 2026 Gaël Fortier

## Disclaimer
This projet is unstable and likely to crash. 
This projet is meant as an artefact for my portfolio.
Avoid using this program.

## CLI file explorer
Essentially, this is the equivalent of the `cd` and `ls` commands in Linux. It requires SDL2 to run as it is used for opening and rendering images in a window. 

There are 3 commands available. When entering the command, only type the command name since if there are more parameters, the program will prompt you for each individual parameters. 

- `set`: opens the directory specified by an absolute path. A second prompt will ask you to specify the absolute path.
- `cd`: opens a child directory. A second prompt will ask you for the number of the entry associated with the desired child directory.
- `quit`: closes the program.

When a number is entered in the command prompt, the program will attempt to get the entry associated with the entered number, then try to open the file as an image and display it in an SDL window. To close the window, press any key. 
