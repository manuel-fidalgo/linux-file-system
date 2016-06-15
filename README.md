Linux file system
This filesystem is a modification of https://lwn.net/Articles/57371/ I added some new features like:                                     
  -Create new files and directories.                                                                                                     
  -Write and read text from the files.                                                                                                                                                                                                                                            
GLP license -> (http://www.gnu.org/licenses/gpl-3.0.txt)                                                                                                                                                                                                                                                                                                                                                                   
Instructions for install->                                                                                                               
Change in the source code the COUNT_MODE flag,use 0 for text, 1 for counters.                                                            
do->                                                                                                                                             mkdir /mnt/assoofs                                                                                                               
        make                                                                                                                             
        sudo ./insert.sh                                                                                                                 
Now the module is inserted and mounted, you will need root privileges for some actions.                                                  


