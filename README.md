# unix-pipe-ipc
C program that implements Unix interprocess communication using pipe, fork, and dup2 to connect two commands — replicating shell pipe behavior using explicit syscalls.

Instructions:

use the following commands to run and/or compile the program.

to compile program use the following command :  make

to run program use the following commands:       
 (commands are below)

./lab012.out sort -r arg0 -pipe uniq -i     -- example: ./lab12.out sort -r words.txt -pipe uniq -i
./lab012.out ps --version -pipe cat -E      -- example: ./lab12.out ps --version -pipe cat -E
./lab012.out cat arg0 -pipe sort            -- example: ./lab12.out cat words.txt -pipe sort
