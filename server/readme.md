bug记录：
1.注册后返回登录界面无法登录，但是直接登录是可以的。已解决，由于注册和主函数分开，注册点击返回按钮会创建新的登录对象，和主函数脱离。发送一个信号就好了。
2.登录时调用SendUserList(rs.m_user_id);俩函数会段错误，因为存在内存中的，服务器下线就消失，需要从数据库导入用户信息进来。原因可能不对
，但是UserInfo *loginer = m_mapIDToUserInfo[id];这行访问了无效的id导致的。
3.**gdb调试要学会**，比cout打印方便。学会看core文件。
4.退出房间发的信号，房间号不对。换成对象成员即可。
5.离开房间再次加入页面消失。
6.有bug，第二个用户登录有几率闪退。
7.**StructRoomListRequest里定义了map**，调试的时候发现无法访问而报错。原因：send函数中先把结构体转换为指针，大小用sizeof来求结构体大小，其他的
结构体因为都是普通类型，char a[]也是，但是定义了map，即使map里内容很大，他也时固定大小，因此出错。
解决方法暂时只想到了两个，一个是在循环体中循环send，缺点是tcp头部和封装的头部信息会多次发送；另一个是定义一个很长的字符，向尾部一点一点添加。
网友推荐**protobuf**

8.暂存bug。退出房间给每个人发信号，这里可能有漏洞；其次，如果退出房间，房间空了要清空链表。已解决。
9.用户强制关闭后，服务端没有清空数据，再次登录没有收到登录信息。
10.第二天登录时，虚拟机无线ip改变，需要更换代码中的ip。此外，.h文件也需要重新make才能识别到。
11.用户离开时需要删除那个用户，用户加入需要添加，要分开实现。可以离开的时候也发一个头部信息提示，让客户端这边删除用户列表，再一个一个添加。
12.listwidget->takeItem(0);删除的时候，传入参数时0，不能是i。

TODO:用户界面的音量调节；主持人转让；静音用户；踢人；主持人退出后，房间自动解散，其他人自动退出房间。
用户退出时，发送下线信号，把房间属性都清空。可以用一个心跳信号检测，因为目前用户强制退出会导致房间号多余显示。


vscode调试方法：
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/server",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            // "preLaunchTask": "C/C++: g++ 生成活动文件",
            // "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}