/*
 * @Author: FengYanBin
 * @Date: 2021-08-18 17:45:51
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-19 09:29:48
 * @Description: file content
 * @FilePath: /server/src/udp_net.cpp
 */

using namespace std;
#define SERVER_PORT 9527
#define BUFF_LEN 47480

#include "../include/udp_net.h"

bool operator<(Addr a, Addr b)
{
    return make_pair(a.sin_addr.s_addr, a.sin_port) < make_pair(b.sin_addr.s_addr, b.sin_port);
}
bool operator==(Addr a, Addr b)
{
    return make_pair(a.sin_addr.s_addr, a.sin_port) == make_pair(b.sin_addr.s_addr, b.sin_port);
}
bool operator!=(Addr a, Addr b)
{
    return !(a == b);
}
ostream &operator<<(ostream &out, Addr a)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &a.sin_addr, ip, sizeof(ip));
    out << "[" << ip << "]:" << a.sin_port;
    return out;
}

bool UdpNet::InitNetWork()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("create socket fail!");
        return false;
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(kUdpServerPort);

    //绑定端口号
    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("Bind Socket Error!");
        return false;
    }

    printf("Init UdpNet Success!\n");

    return true;
}

void UdpNet::eventLoop()
{

    // 重定向 clog
    ofstream file = ofstream("./log.txt");
    streambuf *x = clog.rdbuf(file.rdbuf());
    clog << "this is a log file" << endl;
    clog << "create time: " << time(nullptr) << endl;

    static char buff[BUFF_LEN];
    Addr currClient;
    socklen_t sock_len = sizeof(currClient);

    // map<int, set<Addr>> rooms;
    // map<Addr, time_t> lastOnlimeTime;

    int recvCnt = 0;
    while (true)
    {
        clog << "-------------------------- " << recvCnt++ << "th:" << endl;
        int recvLen = recvfrom(sockfd, buff, BUFF_LEN, 0, (struct sockaddr *)&currClient, &sock_len);
        if (recvLen < 0)
        {
            break;
        }

        if (buff[0] == 'F' or buff[0] == 'f')
        {
            int roomId = *reinterpret_cast<int *>(buff + 1);
            rooms[roomId].insert(currClient);
            lastOnlimeTime[currClient] = time(nullptr);
            buff[4] = buff[0];
            for (auto &client : rooms[roomId])
            {
                if (time(nullptr) - lastOnlimeTime[client] < 3)
                {
                    auto sendCnt = SEND_TO(client, buff + 4, recvLen - 4);
                    clog << currClient << " ---> " << client << " " << sendCnt << " bytes" << endl;
                }
            }
        }
    }
}

int UdpNet::SendData(int clientfd, char *szbuf, int nlen)
{
    if (send(clientfd, (const char *)&nlen, sizeof(int), 0) < 0)
        return false;
    if (send(clientfd, szbuf, nlen, 0) < 0)
        return false;
    return true;
}

void UdpNet::UnInitNetWork()
{
    close(sockfd);
}
