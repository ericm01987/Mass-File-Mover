# Mass-File-Mover
GUI Windows program that takes a source directory, extracts all files in source directory, and either moves all files in source directory to a new location or moves all files in the sub-directories to the source directory

Basic functions: The mass file mover moves files from the source directory and its subfolders to the destination directory. This can also be used to extract all files from the source directory's subfolders to the source directory itself by providing both the source directory field and destination directory field with the same path.

Naming: If the "file name" field is left blank and "keep original file names" isn't checked, the names of the files will be numbers (0.jpg, 1.jpg, etc.) 
If "keep original file names" is checked and a duplicate filename is found, MFM will add a number after the filename (a.jpg, a(1).jpg, etc.). 

Blacklist and whitelist: The whitelist and blacklist will exclusively use or exclude files on the list, respectively. The list can be modified from the options menu or from the INI fle.
