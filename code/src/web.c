#include <stdio.h>
#include <string.h>    //strlen, strcmp
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>  //for threading , link with lpthread
#include <sys/types.h>
#include <ifaddrs.h>

#include "web.h"


#define MESSAGE_BUFFER_SIZE  200

static char myIP[NI_MAXHOST] = "";
const unsigned short broadcastPort = 33333;
const unsigned short receivePort = 44444;


char* getMyIP (char* interface)
{
    struct ifaddrs *ifaddr, *ifa;
    int s;

    if (getifaddrs(&ifaddr) == -1) 
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;  
        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),myIP, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if((strcmp(ifa->ifa_name,interface)==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
    freeifaddrs(ifaddr);
    return myIP;
}

void broadcast_msg(char *mess, char *broadcastIP){
   int sock;
   struct sockaddr_in broadcastAddr;
   int broadcastPermission;
   int sendStringLen;

   if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
       fprintf(stderr, "socket error");
       exit(1);
   }

   broadcastPermission = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,sizeof(broadcastPermission)) < 0){
       fprintf(stderr, "setsockopt error");
       exit(1);
   }

   /* Construct local address structure */
   memset(&broadcastAddr, 0, sizeof(broadcastAddr));
   broadcastAddr.sin_family = AF_INET;
   broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
   broadcastAddr.sin_port = htons(broadcastPort);

   sendStringLen = strlen(mess);

    /* Broadcast mess in datagram to clients */
    if (sendto(sock, mess, sendStringLen, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != sendStringLen){
        fprintf(stderr, "sendto error");
        exit(1);
    }
}

const char* getUDPmessage(void)
{
    struct sockaddr_in si_me, si_other;
    int s, slen = sizeof(si_other), recv_len;
    static char message[MESSAGE_BUFFER_SIZE] = "";

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket");
        exit(1);
    }
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(receivePort);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        perror("bind");
        exit(1);
    }

    if ((recv_len = recvfrom(s, message, MESSAGE_BUFFER_SIZE, 0, (struct sockaddr *) &si_other, (socklen_t *)(&slen))) == -1)
    {
        perror("recvfrom()");
        exit(1);
    }
    message[recv_len] = 0;
    close(s);
    return message;
}
