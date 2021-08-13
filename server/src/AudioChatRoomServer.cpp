#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
//#include <pthread.h>
#include <time.h>
#include <map>
#include <utility>
#include <vector>
#include <set>
#include <fstream>

#define SERVER_PORT 12345
#define BUFF_LEN 47480
using namespace std;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct sockaddr_in Addr;
int server_fd;
#define SEND_TO(client, base, len) sendto(server_fd, (base), (len), 0, (struct sockaddr *)&(client), sizeof(Addr));

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

int main()
{
    //      freopen("log.txt", "w", stdout);
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0)
    {
        cout << "create socket fail!" << endl;
        return -1;
    }

    struct sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(addr_server));

    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_server.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0)
    {
        cout << "bind fail!" << endl;
        return -1;
    }

    char buf[BUFF_LEN];
    struct sockaddr_in addr_client;
    socklen_t sock_len = sizeof(addr_client);
    cout << "sock_len = " << sock_len << endl;

    while (true)
    {
        int recv_cnt = recvfrom(
            server_fd, buf, BUFF_LEN, 0,
            (struct sockaddr *)&addr_client, &sock_len);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_client.sin_addr, ip, sizeof(ip));
        printf("ip: %s ", ip);
        cout << "port:" << htons(addr_client.sin_port) << endl;
        cout << "sock_len = " << sock_len << endl;
        if (recv_cnt == -1)
        {
            cout << "recv fail" << endl;
            break;
        }

        //              if ( rand()%10==1 ) {
        //                      /*
        for (int i = 0; i < recv_cnt; i++)
        {
            if (i % 16 == 0)
            {
                cout << endl;
            }
            printf("%d\t", buf[i]);
        }
        cout << endl;
        //                      */
        //              }

        //              for ( int i=0; i<1; i++){
        int sendret = sendto(server_fd, buf, recv_cnt, 0,
                             (struct sockaddr *)&addr_client, sock_len);
        cout << "sendret = " << sendret << endl;
        //              }
    }
}
