/*
 * @Author: FengYanBin
 * @Date: 2021-08-11 14:25:50
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-11 14:32:14
 * @Description: file content
 * @FilePath: /server/test.cpp
 */
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
using namespace std;

int main()
{

    /*  chrono::system_clock::time_point与std::time_t类型可相互函数
 *  chrono::system_clock::to_time_t()   
 *  chrono::system_clock::from_time_t()     
 */

    chrono::system_clock::time_point now = chrono::system_clock::now(); //当前时间time_point格式
    std::time_t nowTime = chrono::system_clock::to_time_t(now);         //转换为 std::time_t 格式
    cout << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %X");     // 2019-06-18 14:25:56

    // oldTime == timeT
}
