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

#define SERVERC_IP "127.0.0.1"
#define ARRAYLEN 4
#define CLIENT_PORT_NUM 25544
#define UDP_PORT_NUM 23544
#define FILEBUFFSIZE 11
#define chartonum(x) (x-'0')

void Die(char *mess) { perror(mess); exit(1); }
void printbuffer();
int main(int argc, char *argv[]) {
    int servercsoc, clientsoc;
    struct sockaddr_in servercaddr, clientaddr;
    char filebuf [FILEBUFFSIZE];
    int buffer[ARRAYLEN][ARRAYLEN];
    int i=0; int j=0;
    for(i=0;i<4;i++){
        for (j=0;j<4;j++){
            buffer[i][j]=0;
        }
    }
    
     /*read and print out txt file information*/
    FILE *fp;
    
    if (!(fp=fopen("serverC.txt","r"))) {
        printf("file open failure\n");
    }
    printf("The Server C is up and running.\n");
    printf("The Server C has the following neighbor information:\n");
    printf("Neighbor------Cost\n");
    int cost=0;
    fseek(fp,0,SEEK_SET);
    fgets(filebuf,11,fp);
    
    while(!feof(fp)){
       printf(filebuf); printf("\n");
        
        cost=chartonum(filebuf[8]);
        for (i=9; i<11; i++) {
            if (isdigit(filebuf[i])) {
                cost=cost*10+chartonum(filebuf[i]);
            }
        }
        
        switch (filebuf[6]) {
            case 'A':
                buffer[2][0]= cost;
                break;
            case 'B':
                buffer[2][1]= cost;
                break;
            case 'C':
                buffer[2][2]= cost;
                break;
            case 'D':
                buffer[2][3]= cost;
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
    if ((servercsoc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        Die("Failed to create socket");
    }
    memset(&clientaddr, 0, sizeof(clientaddr));       /* Clear struct */
    clientaddr.sin_family = AF_INET;                  /* Internet/IP */
    clientaddr.sin_addr.s_addr = inet_addr(SERVERC_IP);  /* IP address */
    clientaddr.sin_port = htons(CLIENT_PORT_NUM);       /* server port */
    /* Establish connection */
    if (connect(servercsoc,
                (struct sockaddr *) &clientaddr,
                sizeof(clientaddr)) < 0) {
        Die("Failed to connect with server");
    }
    
    int buflen=sizeof(buffer);
    //printf("len=%d\n",addrlen);
    if (send(servercsoc, buffer, buflen, 0) != buflen) {
        Die("Mismatch in number of sent bytes");
    }
    
    printf("The Server C finishes sending its neighbor information to the client with TCP port number %d and IP address %s\n", ntohs(clientaddr.sin_port),inet_ntoa(clientaddr.sin_addr));
    
    int getsock_check; addrlen=sizeof(servercaddr);
    if ((getsock_check=getsockname(servercsoc,(struct sockaddr *)&servercaddr, (socklen_t *)&addrlen)) == -1) {
        perror("getsockname"); exit(1);
    }
    fprintf(stdout, "For this connection with the Client, the Server C has TCP port number %d and IP address %s.\n",ntohs(servercaddr.sin_port),inet_ntoa(servercaddr.sin_addr));
    
    /*create UDP connection*/
    int udpserversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in udpserveraddr,udpclientaddr;
    udpserveraddr.sin_family = AF_INET;
    udpserveraddr.sin_port = htons(UDP_PORT_NUM);
    udpserveraddr.sin_addr.s_addr = inet_addr(SERVERC_IP);
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
    

    printf("The Server C has received the network topology from the Client with UDP port number %d and IP address %s as follows:\n",ntohs(udpclientaddr.sin_port),inet_ntoa(udpclientaddr.sin_addr));
    
    printbuffer(buffer);
    printf("For this connection with Client, The Server C has UDP port number %d and IP address %s.\n"
           ,ntohs(udpserveraddr.sin_port),inet_ntoa(udpserveraddr.sin_addr));
    close(servercsoc);close(clientsoc);
    close(udpserversocket);
    exit(0);
}

/*print out edge--cost message*/
void printbuffer(int buffer[ARRAYLEN][ARRAYLEN]){
    printf("Edge------Cost\n");
    int i=0; int j=0;
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