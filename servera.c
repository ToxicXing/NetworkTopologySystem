#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <ctype.h>

#define SERVERA_IP "127.0.0.1"
#define ARRAYLEN 4
#define CLIENT_PORT_NUM 25544
#define UDP_PORT_NUM 21544
#define FILEBUFFSIZE 11
#define chartonum(x) (x-'0')

void Die(char *mess) { perror(mess); exit(1); }
void printbuffer();
int main(int argc, char *argv[]) {
    int serverAsoc, clientsoc;
    struct sockaddr_in serverAaddr, clientaddr;
    char filebuf [FILEBUFFSIZE];
    int buffer[ARRAYLEN][ARRAYLEN];
    int i=0,j=0;
    for(i=0;i<4;i++){
        for (j=0;j<4;j++){
            buffer[i][j]=0;
        }
    }
    
    printf("The Server A is up and running.\n");
    printf("The Server A has the following neighbor information:\n");
    FILE *fp;
    /*read and print out txt file information*/
    if (!(fp=fopen("serverA.txt","r"))) {
        printf("file open failure\n");
    }
    printf("Neighbor------Cost\n");
    int cost=0;
    fseek(fp,0,SEEK_SET);
    fgets(filebuf,11,fp);
    
    while(!feof(fp)){
         printf(filebuf);printf("\n");
        
        cost=chartonum(filebuf[8]);
        for (i=9; i<11; i++) {
            if (isdigit(filebuf[i])) {
                cost=cost*10+chartonum(filebuf[i]);
            }
        }
        
        switch (filebuf[6]) {
            case 'A':
                buffer[0][0]= cost;
                break;
            case 'B':
                buffer[0][1]= cost;
                break;
            case 'C':
                buffer[0][2]= cost;
                break;
            case 'D':
                buffer[0][3]= cost;
                break;
            default:
                break;
        }
        
        fseek(fp,1,SEEK_CUR);
        fgets(filebuf,11,fp);
    }
    fclose(fp);
    unsigned int addrlen;
    int received = 0;
    if (argc != 1) {
        fprintf(stderr, "Usage error.\n");
        exit(1);
    }
    /* Create the TCP socket */
    if ((serverAsoc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        Die("Failed to create socket");
    }
    memset(&clientaddr, 0, sizeof(clientaddr));       /* Clear struct */
    clientaddr.sin_family = AF_INET;                  /* Internet/IP */
    clientaddr.sin_addr.s_addr = inet_addr(SERVERA_IP);  /* IP address */
    clientaddr.sin_port = htons(CLIENT_PORT_NUM);       /* server port */
    /* Establish connection */
    if (connect(serverAsoc,
                (struct sockaddr *) &clientaddr,
                sizeof(clientaddr)) < 0) {
        Die("Failed to connect with server");
    }
    
    int buflen=sizeof(buffer);
    //printf("len=%d\n",addrlen);
    if (send(serverAsoc, buffer, buflen, 0) != buflen) {
        Die("Mismatch in number of sent bytes");
    }
    
    printf("The Server A finishes sending its neighbor information to the client with TCP port number %d and IP address %s\n", ntohs(clientaddr.sin_port),inet_ntoa(clientaddr.sin_addr));
    
    int getsock_check; addrlen=sizeof(serverAaddr);
    if ((getsock_check=getsockname(serverAsoc,(struct sockaddr *)&serverAaddr, (socklen_t *)&addrlen)) == -1) {
        perror("getsockname"); exit(1);
    }
    fprintf(stdout, "For this connection with the Client, the server A has TCP port number %d and IP address %s.\n",ntohs(serverAaddr.sin_port),inet_ntoa(serverAaddr.sin_addr));
    
    /*create UDP connection*/
    int udpserversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int udpclientsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in udpserveraddr,udpclientaddr;
    udpserveraddr.sin_family = AF_INET;
    udpserveraddr.sin_port = htons(UDP_PORT_NUM);
    udpserveraddr.sin_addr.s_addr = inet_addr(SERVERA_IP);
    if(bind(udpserversocket, (struct sockaddr *)&udpserveraddr, sizeof(udpserveraddr)) < 0){
        Die("Failed to bind the server socket");
    }

    while (received < buflen) {
        int bytes = 0; addrlen=sizeof(udpclientaddr);
        if((bytes = recvfrom(udpserversocket, buffer, 255, 0, (struct sockaddr *)&udpclientaddr, &addrlen)) <0){
            Die("Failed to receive initial bytes from client");
        }
        //printf("bytes= %d\n",bytes);
        received += bytes;
    }
    

    printf("The server A has received the network topology from the Client with UDP port number %d and IP address %s as follows:\n",ntohs(udpclientaddr.sin_port),inet_ntoa(udpclientaddr.sin_addr));
    
    printbuffer(buffer);
    printf("For this connection with Client, The Server A has UDP port number %d and IP address %s.\n"
           ,ntohs(udpserveraddr.sin_port),inet_ntoa(udpserveraddr.sin_addr));
    close(serverAsoc);close(clientsoc);
    close(udpserversocket);
    exit(0);
}

void printbuffer(int buffer[ARRAYLEN][ARRAYLEN]){    /*print out edge cost message*/
    printf("Edge------Cost\n");
    int i=0,j=0;
    for (i=0; i<ARRAYLEN; i++) {
        for (j=i; j<ARRAYLEN; ++j) {
            if (buffer[i][j]>0) {
                switch (i) {
                    case 0:
                        printf("A");
                        break;
                    case 1:
                        printf("B");
                        break;
                    case 2:
                        printf("C");
                        break;
                    case 3:
                        printf("D");
                        break;
                    default:
                        break;
                }
                switch (j) {
                    case 0:
                        printf("A");
                        break;
                    case 1:
                        printf("B");
                        break;
                    case 2:
                        printf("C");
                        break;
                    case 3:
                        printf("D");
                        break;
                    default:
                        break;
                }
                printf("      ");
                printf("%d\n",buffer[i][j]);
            }
        }
    }
}