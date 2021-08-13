/*
 * @Author: FengYanBin
 * @Date: 2021-08-10 11:26:39
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-12 14:32:40
 * @Description: file content
 * @FilePath: /server/main.cpp
 */
#include "./include/mysql_server.h"
#include "./include/tcp_kernel.h"
#include <chrono>
#include <ctime>
#include <iomanip>

int main(int argc, char *argv[])
{
    TcpKernel *pKernel = new TcpKernel;

    if (pKernel->Open())
    {
        while (1)
        {
            printf("Server Running! Time:");
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); //当前时间time_point格式
            std::time_t nowTime = std::chrono::system_clock::to_time_t(now);              //转换为 std::time_t 格式
            cout << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %X") << endl;       // 2019-06-18 14:25:56
            sleep(kTimeOut);
        }
    }
    pKernel->Close();

    return 0;
}
