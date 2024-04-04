#include <iostream>
#include <conio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#define MSGBUFSIZE 256
using std::thread;
bool MODE;
bool PRG;
void trg()
{
    std::cout << "Put 'Q' to exit";
    int c;
    while (1)
    {
        c = _getch();
        if (c == 'M' || c == 'm')
        {
            MODE = true;
        }
        if (c == 'U' || c == 'u')
        {
            MODE = false;
        }
        if (c == 'Q' || c == 'q')
        {
            PRG = false;
        }
    }
}
int main(int argc, char* argv[])
{
    PRG = true;
    //argv[0] = адрес мультикаст
    //argv[1] = порт мультикаст
    //argv[3] = адрес уникаст
    //argv[4] = порт уникаст
    char* group = argv[0];
    int port_multicast = atoi(argv[1]);
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) 
    {
        perror("socket");
        return 1;
    }
    u_int yes = 1;
    if (
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0) 
    {
        perror("Reusing ADDR failed");
        return 1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_multicast);
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0) 
    {
        perror("setsockopt");
        return 1;
    }
    char* unicast = argv[3];
    int port_unicast = atoi(argv[4]);
    int fd2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd2 < 0)
    {
        perror("socket");
        return 1;
    }
    if (
        setsockopt(fd2, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0)
    {
        perror("Reusing ADDR failed");
        return 1;
    }
    struct sockaddr_in addr2;
    memset(&addr2, 0, sizeof(addr2));
    addr.sin_family = AF_INET;
    addr2.sin_addr.s_addr = htonl(INADDR_ANY);
    addr2.sin_port = htons(port_unicast);
    if (bind(fd, (struct sockaddr*)&addr2, sizeof(addr2)) < 0) {
        perror("bind");
        return 1;
    }
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt");
        return 1;
    }
    while (PRG)
    {
        std::cout << "Put 'M' for listening MULTICAST \n Put 'U' for listening UNICAST";
        c = _getch();
        if (c == 'M' || c == 'm')
        {
            MODE = true;
            break;
        } 
        if (c == 'U' || c == 'u')
        {
            MODE = false;
            break;
        }
    }
    thread trigger (trg);
    int nbytes;
    while (PRG) {
        char msgbuf[MSGBUFSIZE];
        int addrlen = sizeof(addr);
        if (MODE)
        {
            nbytes = recvfrom(fd,msgbuf,MSGBUFSIZE,0,(struct sockaddr*)&addr,&addrlen);
            
        }
        else
        {
            nbytes = recvfrom(fd2, msgbuf, MSGBUFSIZE, 0, (struct sockaddr*)&addr2, &addrlen);
        }
        if (nbytes < 0)
        {
            perror("recvfrom");
            return 1;
        }
        msgbuf[nbytes] = '\0';
        puts(msgbuf);
        if (MODE)
        {
            send(fd2, msgbuf, MSGBUFSIZE, 0);

        }
        else
        {
            send(fd1, msgbuf, MSGBUFSIZE, 0);
        }
    }
    trigger.detach();
    close(fd);
    close(fd2);
    return 0;
}



