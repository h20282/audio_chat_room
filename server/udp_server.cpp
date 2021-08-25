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
using namespace std;
#define SERVER_PORT 9528
#define BUFF_LEN 47480

typedef struct sockaddr_in Addr;

bool operator < (Addr a, Addr b){
    return make_pair(a.sin_addr.s_addr, a.sin_port) < make_pair(b.sin_addr.s_addr, b.sin_port);
}
bool operator == (Addr a, Addr b){
    return make_pair(a.sin_addr.s_addr, a.sin_port) == make_pair(b.sin_addr.s_addr, b.sin_port);
}
bool operator != (Addr a, Addr b){
    return !(a==b);
}
ostream& operator << (ostream& out, Addr a) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &a.sin_addr, ip, sizeof(ip));
    out << "[" << ip << "]:" << a.sin_port;
    return out;
}


int serverFd;
#define SEND_TO(client, base, len) sendto(serverFd, (base), (len), 0, (struct sockaddr*)&(client), sizeof(Addr));


int main(){
    serverFd = socket(AF_INET, SOCK_DGRAM, 0);

    // 重定向 clog
    ofstream file = ofstream( "log.txt" );
    streambuf *x = clog.rdbuf(file.rdbuf());
    clog << "this is a log file" << endl;
    clog << "create time: " << time(nullptr) << endl;

    // 建立socket
    serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( serverFd < 0 ) {
        cout << "create socket fail!" << endl;
        return -1;
    }

    // 设置：ipv4, 119.91.116.26, 端口号
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);

    // 绑定
    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "bind fail!" << endl;
        return -1;
    }

    static char buff[BUFF_LEN];
    Addr currClient;
    socklen_t sock_len = sizeof(currClient);

    map<int, set<Addr> > rooms;
    map<Addr, time_t > lastOnlimeTime;

    int recvCnt = 0;
    while ( true ) {
        clog << "-------------------------- " << recvCnt++ << "th:" << endl;
        int recvLen = recvfrom( serverFd, buff, BUFF_LEN, 0, (struct sockaddr*)&currClient, &sock_len );
        if (recvLen<0){
            break;
        }

        if (buff[0]=='F' or buff[0]=='f'){
            cout << "a frame from <<" << currClient << ">> len = " << recvLen << endl;
            int roomId = *reinterpret_cast<int*>(buff+1);
			//合理性检测：房间号必须为5位数字[)
			if (roomId<1e5 || roomId>=1e6){
				continue;
			}
            rooms[roomId].insert(currClient);
            lastOnlimeTime[currClient] = time(nullptr);
            buff[4] = buff[0];
            for ( auto& client : rooms[roomId] ) {
                if (time(nullptr) - lastOnlimeTime[client] < 3 ){
                    auto sendCnt = SEND_TO(client, buff+4, recvLen-4);
                    clog << currClient << " ---> " << client << " " << sendCnt << " bytes" << endl;

                }
            }
        }
    }
}
