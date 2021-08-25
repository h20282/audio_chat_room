/*
 * @Author: FengYanBin
 * @Date: 2021-08-18 17:46:25
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-19 09:20:27
 * @Description: file content
 * @FilePath: /server/include/udp_net.h
 */
/*
 * @Author: FengYanBin
 * @Date: 2021-08-13 10:37:20
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-13 11:01:42
 * @Description: file content
 * @FilePath: /server/include/udp_net.h
 */
#ifndef _UDPNET_H
#define _UDPNET_H

#include <iostream>
//#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
//#include <unistd.h>
#include <stdlib.h>
//#include <pthread.h>
#include <time.h>
#include <map>
#include <utility>
#include <vector>
#include <set>
#include <fstream>

#include "pack_def.h"
#include "tcp_net.h"

typedef struct sockaddr_in Addr;

#define SEND_TO(client, base, len) sendto(sockfd, (base), (len), 0, (struct sockaddr *)&(client), sizeof(Addr));

class UdpNet
{
public:
    UdpNet() {}

    bool InitNetWork();
    void UnInitNetWork();

    int SendData(int, char *, int);

    void eventLoop();

public:
    int sockfd;
    int clientfd;

    struct sockaddr_in serveraddr;
    map<int, set<Addr>> rooms;
    map<Addr, time_t> lastOnlimeTime;
};

#endif
