Disk-Scheduling-Simulation
program name: DiskSimulation
Mohammad Belbesi

===Description===
This program is a simulation of managing the disk system by the method fsInode structure emulating a file system 
similar to what is used in traditional UNIX-style systems ,The file system utilizes direct blocks, single indirect 
blocks,and double indirect blocks to store data.
We will simulate the way that the files' names, their location, and their contents are organized on the hard disk.   
The file's parts are saved on small units - the blocks, which are in the size of 4 chars in this simulation while 
the disk size is 256 chars and we will use one folder. 
We have three data structures/class 
Fsfile which saving the information about the files. 
FileDescriptor - saving the pointer to the FsFile of the files and the files' name, 
and the FsDisk, the disk itself, which saves the information about the disk. 
The user can create (3), open (4), close (5), write (6), read (7), delete (8), copy(9), rename(10), print (1) or
delete files and content from the disk and exit (0) or format (2) the disk
according to numbered options 0-10. 

==Program Files==
DiskSimulation.cpp

==functions==
we have eight main functions: 
1-CreateFile - getting the file name, creates new fsFile, and update mainDir.
2-WriteToFile -  write to the file according to free space in the disk 
3-ReadFromFile -  read from the file according to the number of given chars
4-OpenFile - getting file name and opens the file and returns the file descriptor 
5-CloseFile - getting file descriptor and closes the file 
6-fsFormat -  formats the disk
7-listAll - print the content of the disk
8-DelFile - delete the file and all its data
9-copyFile- copy the file and all its data
10-rename- rename a file
other private functions:
delete_index
next_index
fileExist
get_the_fd

==How to compile?==
Compile: g++ DiskSimulation.cpp -o DiskSimulation
Run: ./DiskSimulation

==Input:== 
Input from the user according to the operations on the disk.
options: 0-10

==Output:== 
The results of the operations including prints and prints for success or failure indications.