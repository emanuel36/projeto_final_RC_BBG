/*
    Simple udp client
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
 
#define SERVER "192.168.1.18"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data
 
struct timeval timeout={1,0}; 

struct sockaddr_in si_other;
int s, i, slen=sizeof(si_other);
char buf[BUFLEN];
char message[BUFLEN];

void die(char *s){
    perror(s);
    exit(1);
}

void send_and_recv(){
    sleep(2);
    setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));     
    //send the message
    if(sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1){
        die("sendto()");
    }
    //receive a reply and print it
    //clear the buffer by filling null, it might have previously received data
    memset(buf, 0x0, BUFLEN);
    //try to receive some data, this is a blocking call
    int recvlen = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen);

    if(recvlen < 0){
        printf("Timeout, attempting retransmission...\n\n");
        send_and_recv();
    }
}
 
int main(void){
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0){
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    while(1){
        printf(">>");
        memset(message, 0x0, BUFLEN);
        gets(message);
        if(strcmp("help", message) == 0){
            printf("\nHELP\n1- ledOn\n2- ledOff\n3- readLed\n4- poweroff\n\n");
            continue;
        }
        send_and_recv();  
        printf("Beaglebone: %s\n", buf);
    }
 
    close(s);
    return 0;
}
