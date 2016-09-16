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

#define ARRAYLEN 4
#define IPADDR "127.0.0.1"
#define TCPPORTNUM 25544
#define MAXPENDING 5
#define NUM_OF_SERVERS 4
#define MAX_INT 255
int buffer[ARRAYLEN][ARRAYLEN];
int matrix[ARRAYLEN][ARRAYLEN];

void initial_buffer_and_matrix();
void selfaddr_check();
void Die(char *mess) { perror(mess); exit(1); }
void printbuffer();
void prim();

int main (int argc, char *argv[]){
    initial_buffer_and_matrix();
    
    int udp_port_num=21544;
    int clientsoc,serversoc;
    int udpclientsocket;
    struct sockaddr_in clientaddr, serveraddr;
    
    if (argc != 1) {
        fprintf(stderr, "Please boot up client by using ./client.\n");
        exit(1);
    }
   
    if ((clientsoc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {  /*create TCP socket*/
        Die("Failed to create socket");
    }

    memset(&clientaddr, 0, sizeof(clientaddr));    /*configure address information for client*/
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = inet_addr(IPADDR);
    clientaddr.sin_port = htons(TCPPORTNUM);
    
    
    
    if (bind(clientsoc, (struct sockaddr *) &clientaddr,     /*bind address information with socket*/
             sizeof(clientaddr)) < 0) {
        Die("Failed to bind the server socket");
    }
    
   
    if (listen(clientsoc, MAXPENDING) < 0) {                 /*listen for incoming connection request*/
        Die("Failed to listen on server socket");
    }
    
    printf("The Client has TCP port number %d and IP address %s\n",
           ntohs(clientaddr.sin_port),inet_ntoa(clientaddr.sin_addr));
    
    
    int times=0; char server_name[4]={'A','B','C','D'};         /*self-changed server names*/
    for (times=0; times<NUM_OF_SERVERS; times++) {              /*receive TCP message from server ABCD*/
        printf("\n");
    unsigned int serverlen = sizeof(serveraddr);
    
    if ((serversoc =
         accept(clientsoc, (struct sockaddr *) &serveraddr,
                &serverlen)) < 0) {
             Die("Failed to accept client connection");
         }
    
    int received=-1;
    if ((received = recv(serversoc, buffer, sizeof(buffer), 0)) < 0) {
        Die("Failed to receive initial bytes from client");
    }
    printf("The Client receives neighbor information from the Server %c with TCP port number %d and IP address %s.\n",server_name[times],ntohs(serveraddr.sin_port),inet_ntoa(serveraddr.sin_addr));
 

    printf("The server %c has the following neighbor information:\n",server_name[times]);   /*print out received message*/
         printf("Neighbor------Cost\n");
        

        int i=0,j=0;
        for(i=0;i<ARRAYLEN;i++){
            for (j=0; j<ARRAYLEN; j++) {
                if (buffer[i][j]>0) {
                    if (buffer[i][j]>0) {
                        switch (j) {
                            case 0:
                                printf("serverA");
                                break;
                            case 1:
                                printf("serverB");
                                break;
                            case 2:
                                printf("serverC");
                                break;
                            case 3:
                                printf("serverD");
                                break;
                            default:
                                break;
                        }
                        printf("      ");
                        printf("%d\n",buffer[i][j]);
                    }
                }
                matrix[i][j]=matrix[i][j] + buffer[i][j];
            }
        }
    printf("For this connection with server %c, the client has TCP port number %d and IP address %s.\n",server_name[times],ntohs(clientaddr.sin_port),inet_ntoa(clientaddr.sin_addr));
        
    }
    
    for(times=0;times<NUM_OF_SERVERS;++times) {                             /*create UDP connection with server ABCD*/
        udpclientsocket= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        struct sockaddr_in udpserveraddr,udpclientaddr;
        udpserveraddr.sin_family = AF_INET;
        udpserveraddr.sin_port = htons(udp_port_num);
        udpserveraddr.sin_addr.s_addr = inet_addr(IPADDR);
        printf("\n");
        int received=sizeof(matrix);                   /*send UDP message*/
        if (sendto(udpclientsocket, matrix, sizeof(matrix), 0, (struct sockaddr *)&udpserveraddr, sizeof(udpserveraddr))!=received){            Die("Failed to send bytes to servers");
        }
        
        printf("The Client has sent the network topology to the Server %c with UDP port number %d and IP address %s as follows:\n",server_name[times],ntohs(udpserveraddr.sin_port),inet_ntoa(udpserveraddr.sin_addr));
        
        printbuffer(matrix);
        
        int addrlen=sizeof(udpclientaddr);int getsock_check;
        if((getsock_check=getsockname(udpclientsocket,(struct sockaddr *)&udpclientaddr, (socklen_t *)&addrlen)) == -1) {
            perror("getsockname"); exit(1);
        }

        
        
        
        printf("For this connection with Server %c, the Client has UDP port number %d and IP address %s.\n",server_name[times],ntohs(udpclientaddr.sin_port),IPADDR);
        udp_port_num+=1000;
    }

    
    close(clientsoc);
    close(serversoc);
    close(udpclientsocket);
    prim(matrix);
}

void initial_buffer_and_matrix(){          /*array initialization*/
    memset(buffer,0,sizeof(buffer));
    memset(matrix,0,sizeof(matrix));
}

void printbuffer(int buffer[ARRAYLEN][ARRAYLEN]){         /*print on screen information*/
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


void prim(int buffer [ARRAYLEN][ARRAYLEN]){                  /*calculate MST*/
    int cost[ARRAYLEN];
    int visited[ARRAYLEN];
    int i,j,position,min,result=0;
    memset(cost,0,sizeof(cost));
    memset(visited,0,sizeof(visited));
    visited[0]=1;position=0;
    
    int t=0; /*row couter*/
    int temp[ARRAYLEN][ARRAYLEN];  /* MST matrix   record every MST link in a matrix*/
    memset(temp,0,sizeof(temp));
    for (i=0; i<=ARRAYLEN; i++) {
        for (j=0; j<=ARRAYLEN; j++) {
            if (buffer[i][j]==0) {
                buffer[i][j]=MAX_INT;
            }
        }
    }
    
    for (i=0; i<ARRAYLEN; i++) {
            cost[i]=buffer[position][i];
    }
    
    for (i=0; i<ARRAYLEN-1; i++) {
        min=MAX_INT;
        for (j=0; j<ARRAYLEN; j++) {
            if (min>cost[j] && visited[j]==0) {          /*looking for minimum cost in cost aray*/
                min=cost[j];
                position=j;
            }
        }
        temp[t][position]=min;
        
        result+=min;

        visited[position]=1;
        for (j=0; j<ARRAYLEN; j++) {
            if (visited[j]==0 && cost[j]>buffer[position][j]) {
                cost[j]=buffer[position][j];
                t=position;
            }
        }
    }
    printf("\nThe client has calculated a tree. The tree cost is %d:\n",result);
    printf("Edge------Cost\n");                 /*print out MST*/
    for (i=0; i<ARRAYLEN; i++) {
        for (j=0; j<ARRAYLEN; ++j) {
            if (temp[i][j]>0) {
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
                printf("%d\n",temp[i][j]);
            }
        }
    }
}