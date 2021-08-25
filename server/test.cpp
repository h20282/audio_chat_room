// /*
//  * @Author: FengYanBin
//  * @Date: 2021-08-11 14:25:50
//  * @LastEditors: FengYanBin
//  * @LastEditTime: 2021-08-11 14:32:14
//  * @Description: file content
//  * @FilePath: /server/test.cpp
//  */
// #include <iostream>
// #include <chrono>
// #include <ctime>
// #include <iomanip>
// using namespace std;

// int main()
// {

//     /*  chrono::system_clock::time_point与std::time_t类型可相互函数
//  *  chrono::system_clock::to_time_t()
//  *  chrono::system_clock::from_time_t()
//  */

//     chrono::system_clock::time_point now = chrono::system_clock::now(); //当前时间time_point格式
//     std::time_t nowTime = chrono::system_clock::to_time_t(now);         //转换为 std::time_t 格式
//     cout << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %X");     // 2019-06-18 14:25:56

//     // oldTime == timeT
// }

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

//typedef void (*sighandler_t)(int);

void someone_func_handler(int param)
{
    printf("hello\n");
    alarm(1);
}

int main(void)
{
    int i;
    sighandler_t handler = someone_func_handler;

    signal(SIGALRM, handler);

    alarm(1);

    for (i = 1; i < 3; i++)
    {
        printf("sleep %d ...\n", i);
        sleep(1);
    }
    return 0;
}