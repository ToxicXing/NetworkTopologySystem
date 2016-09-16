NAME:Da Xing
STUDENT ID:2069220544

What I’ve done in the assignment:
C language.
In the assignment, I’ve finished both phase 1, 2 and 3 without using fork to generate concurrent process. The onscreen message is exactly the same as requirements. No dynamic port number is hardcoded and the whole system works well no matter on Windows, UNIX or MAC OS.

FILES:
Five code files in total: client.c, servera.c, serverb.c, serverc.c, serverd.c (all small letter)


client.c: create TCP socket first and listen for any TCP connections. Once connected, receive TCP messages from all four servers one by one. The content of the message is a adjacent matrix of each server. Client then add all four matrix up to get the topology of the whole network. Next, client will create UDP connection to all four servers and send the whole topology information as UDP message which is the summed up adjacent matrix to each of them. During receiving and sending, on screen message is shown exactly the same as the requirement. 

servera.c: Read from local txt file “serverA.c” and generate its own adjacent matrix and print it on the screen. Create a TCP connection with client. Send the matrix as TCP message and create a UDP Datagram, waiting for UDP message from client. Once received UDP message from client, which is the whole topology of the network, this server will print it on the screen.

serverb.c, serverc.c, serverd.c: works exactly the same way as servera.c with one thing different that their local txt files are “serverB.txt”, “serverC.txt”, “serverD.txt” correspondingly.

COMPILING AND RUNNING PROCEDURE:
Before compile please make sure all the four local txt files and five code files exist.

Compile all files in any sequence you like.

Run client first and then servera and then serverb and then serverc and then served. The compiling sequence is not strict, but the running sequence must strictly follow client—>servera—>serverb—>serverc—>serverd.
e.g:
compiling command line: gcc -o -client client.c
running command line: ./client
Do not need any other parameters in command line.

The format of all the message exchanged:
No matter TCP or UDP, all the message exchanged between client and servers are matrix implemented by 2-D array.

CODE REUSING:
Following part are implemented by code reusing:
1. read and print out receiving message after communication between servers and client. [void printfbuffer()]
2. print out error message. [void Die()]
3. buffer and matrix initialization in client.c. [void initial_buffer_and _matrix()]



