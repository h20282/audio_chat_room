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
13.现在时如果用户创建了房间，没有点击关闭关掉，突然掉线检测不到。
14.右键弹出菜单的问题在于要重写函数要写在userlist里，不能到主界面写。房间列表和用户列表共用时，可以考虑继承，这里暂时放两个把。
15.list删除元素不一定O(1)，你可能需要一个一个找。此外，还有迭代器失效的问题。需要it = lst.erase(it);然后在循环体里++it。
16.用户异常退出时，服务端无法检测到。
17.特别弱智的一个问题。this->itemAt(mapFromGlobal(QCursor::pos()))->text()捕获的是当前鼠标位置，我弹出提示框后在捕捉自然会报错。
18.全局函数加extern问题，和那个定义const char*类似，一起看一下。我用了线程池那块的方法，把函数作为静态变量，然后转换为类对象，调用非静态方法。
19.但是调用非静态方法不能访问成员变量，不会解决。难道只能管道发送信号，主线程接受了吗？
20.用户关闭某人声音这块，感觉需要在音频头部加一个静音标志。客户端自己有一个hash，存放了你静音的所有用户id。接受的时候，查找hash，看有没有被静音。所以只要在客户端实现就行了。
21.声音来源选择的话，chatroom里面时audioread类，我们可以调用函数ResumeAudio来重启音频，并在这个函数内将format对应的device修改。可以通过判断
deviceInfo.deviceName()是否为combox那个item的文字来判断。
22.调用方法的时候，一定要看对象有没有初始化！！！我这里在加入房间后才初始化音频，因此在用户界面暂时无法设置声音来源。而且第二次打开声音才行。
23.一定要注意迭代器失效问题，太坑了，而且不好调试。失效后不能再次利用it这个迭代器进行操作！！！

TODO:用户界面的音量调节；麦克风设置
用户退出时，发送下线信号，把房间属性都清空。可以用一个心跳信号检测，因为目前用户强制退出会导致房间号多余显示。目前没有发送关闭信号，虽然用户退出，但是
服务器用户列表里还记录了用户信息，不过不影响。
目前影响大的是从房间页面强退了，房间列表不会更新。
完成了心跳检测(用状态来完成的，测试时间的方式不行，不知道为啥)，禁止重复登录功能。

bug记录：
解除静音只能解除一次，第二次无效。
刷新房主列表出现崩溃。

token:s3zwFvSYKYSd2wmvq-z5  vscode_sync
token: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    cvte-vm


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