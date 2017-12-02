/*
    Simple udp server
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "embedded_linux.h" 

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
 
void die(char *s){
    perror(s);
    exit(1);
}
 
int main(void){
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    char resposta[100];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
        die("bind");
    }
     
    //BEAGLEBONE
    init_gpio(44, OUTPUT);
    set_gpio_low(44);
    int cont = 0;        
    //keep listening for data
    while(1){
        memset(buf, 0x0, BUFLEN);
        printf("Waiting for data...");
        fflush(stdout);
         
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1){
            die("recvfrom()");
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n", buf);

        //###BEAGLEBONE###
        if(!strcmp(buf, "ledOn")){
            memset(resposta, 0x0, 100);
        	set_gpio_high(44);
            strcpy(resposta, "Led ligado com sucesso");
        }

        else if(!strcmp(buf, "ledOff")){
            memset(resposta, 0x0, 100);
            set_gpio_low(44);
            strcpy(resposta, "Led desligado com sucesso");
        }

        else if(!strcmp(buf, "readLed")){
            memset(resposta, 0x0, 100);
        	if(get_value(44)){
                strcpy(resposta, "O led está ligado!");
            }else{
                strcpy(resposta, "O led está desligado!");
            }
        }

        else if(!strcmp(buf, "poweroff")){
            memset(resposta, 0x0, 100);
            strcpy(resposta, "O sistema será desligado neste instante...");
            if(sendto(s, resposta, 100, 0, (struct sockaddr*) &si_other, slen) == -1){
                die("sendto()");
            }
            system("poweroff");
        }

        else{
            memset(resposta, 0x0, 100);
        	strcpy(resposta, "Comando Inválido!");
        }
         
        //now reply the client with the same data
        cont++;
        if(cont % 2){
            if(sendto(s, resposta, strlen(resposta), 0, (struct sockaddr*) &si_other, slen) == -1){
                die("sendto()");
            }
        }
    }
    close(s);
    return 0;
}
