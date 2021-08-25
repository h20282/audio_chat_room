/*
 * @Author: FengYanBin
 * @Date: 2021-08-10 11:26:39
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-19 09:20:00
 * @Description: file content
 * @FilePath: /server/main.cpp
 */
#include "./include/mysql_server.h"
#include "./include/tcp_kernel.h"
#include "./include/udp_net.h"
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>

TcpKernel *pKernel = new TcpKernel;
void f(){
	while(1){
		getchar();
		pKernel->printTest();
	}
}

int main(int argc, char *argv[])
{

	thread t(f);

	time_t lastTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (pKernel->Open())
    {
        while (1)
        {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); //当前时间time_point格式
            std::time_t nowTime = std::chrono::system_clock::to_time_t(now);              //转换为 std::time_t 格式

			if (nowTime - lastTime > kTimeOut) {
				 lastTime = nowTime;
				 cout << "Server Running! Time:" << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %X") << endl;       // 2019-06-18 14:25:56
			}

            sleep(kTimeOut);
        }
    }
	t.join();

    pKernel->Close();
    return 0;
}
