#include "../include/tcp_kernel.h"
#include "../include/pack_def.h"
#include <stdio.h>
#include <sys/time.h>

using namespace std;

static const ProtocolMap m_ProtocolMapEntries[] =
    {
        {kPackRegisterQequest, &TcpKernel::Registerequest},
        {kPackLoginQequest, &TcpKernel::Loginequest},
        {kPackCreateRoomQequest, &TcpKernel::CreateRoomequest},
        {kPackJoinRoomQequest, &TcpKernel::JoinRoomequest},
        {kPackQuitRoomQequest, &TcpKernel::QuitRoomequest},
        {kPackRoomListRefreshHeadRequest, &TcpKernel::RefreshRoomList},
        {kPackTransferUserQequest, &TcpKernel::TransferUserRequset},
        {kPackKickOutOfUserQequest, &TcpKernel::KickOutOfUserRequset},
        {kPackHeartDetect, &TcpKernel::HeartDetect},
        {kPackMuteOneUserQequest, &TcpKernel::MuteOneUserRequset},
        {kPackUnmuteQequest, &TcpKernel::UnMuteRequset},
        {kPackRoomMemberQequest, &TcpKernel::SendUserList},
        {kPackOfflineRequest, &TcpKernel::Offlineequest},
        {0, 0}};

void TcpKernel::worker(int sig)
{
    pThis->Timer();
}

TcpKernel *TcpKernel::pThis = nullptr;

void TcpKernel::printTest()
{
	cout << "输出当前用户!" << endl;
	for (auto it = m_mapIDToUserInfo.begin(); it != m_mapIDToUserInfo.end(); ++it) {
		cout << "user:" << it->second->m_user_name << endl;
	}

	cout << "输出当前房间!" << endl;

	for (auto it = m_mapRoomIDToUserList.begin(); it != m_mapRoomIDToUserList.end(); ++it) {
		cout << "房间号：" << it->first << "用户列表如下：" << endl;

		auto lst = it->second;
		for (auto iter = lst.begin(); iter != lst.end(); ++iter) 
		{
			cout << "user:" << (*iter)->m_user_name << endl;
		}
	}
}

void TcpKernel::Timer()
{
    //每隔一段时间，遍历房间所有人，查看上次发送心跳的时间。看是否大于超时时间
    for (auto it = m_mapIDToUserInfo.begin(); it != m_mapIDToUserInfo.end();)
    {
        UserInfo *user = it->second;

        if (user->m_id == 3 || user->m_id == 4 || user->m_id == 5)
        {
            ++it;
            continue;
        }

        int user_id = it->first;
        int room_id = user->m_room_id;
        int user_fd = user->m_fd;
        //时间大于超时时间
        if (user->m_state == 1)
        {
            user->m_state = 0;
            ++it;
            continue;
        }
        else
        {
            //认为掉线
            cout << "user:" << user->m_user_name << "掉线！" << endl;
            m_mapIDToUserInfo.erase(it++);
			//掉线的用户没有加入房间，删除用户即可。
            if (m_mapRoomIDToUserList.find(room_id) == m_mapRoomIDToUserList.end())
            {
				cout << "掉线的用户没有加入房间！" << endl;
                continue;
            }
            auto lst = m_mapRoomIDToUserList[room_id];
			if (lst.empty()) {
				m_mapRoomIDToUserList.erase(room_id);
				continue;
			}
            StructQuitRoomResponse rs;
            StructRoomMemberResponse member_rs;
            //告诉每一个非离开的人，清空用户列表
            StructRoomMemberHeaderResponse head_rs;
            //房主的话关闭房间
            if (user_id == lst.front()->m_id)
            {
                //房主掉线，关闭房间
                //遍历链表，让每一个用户都退出当前房间
                auto iter = lst.begin();
                while (iter != lst.end())
                {
                    UserInfo *inner = *iter;
                    rs.m_user_id = inner->m_id;
                    strcpy(rs.szUserName, inner->m_user_name);
                    m_tcp->SendData(inner->m_fd, (char *)&rs, sizeof(rs));
                    ++iter;
                }
                //遍历完毕，清空链表，删除hash对应内容
				delete user;
                lst.clear();
                m_mapRoomIDToUserList.erase(room_id);
            }
            else
            {
                //不然的话就告诉用户有人掉线
                //遍历list，告诉每一个用户有一个用户离开了
                auto iter = lst.begin();
                while (iter != lst.end())
                {
                    UserInfo *inner = *iter;
                    //发给每一个用户离开人的信息，其实就是更新用户列表
                    if (inner->m_id != user_id) //不需要发给自己
                    {
                        //需要先让每一个用户清空，然后再遍历链表发给用户。
                        head_rs.m_user_id = inner->m_id;
                        this->SendMsgToOnlineClient(inner->m_id, (char *)&head_rs, sizeof(head_rs));
                        for (auto iters = lst.begin(); iters != lst.end(); ++iters)
                        {
                            if (user_id != (*iters)->m_id) //退出房间的那个人不需要再列中中显示
                            {
                                member_rs.m_user_id = (*iters)->m_id;
                                memset(member_rs.m_sz_user, 0, kMaxSize);
                                strcpy(member_rs.m_sz_user, (*iter)->m_user_name);
                                this->SendMsgToOnlineClient(inner->m_id, (char *)&member_rs, sizeof(member_rs));
                            }
                        }
                        ++iter;
                    }
                    else
                    { //从list移除这个节点
                        iter = lst.erase(iter);
                        if (lst.empty())
                            m_mapRoomIDToUserList.erase(room_id);
                    }
                }

                //更新map
				delete user;
                if (!lst.empty())
                    m_mapRoomIDToUserList[room_id] = lst;
                m_tcp->SendData(user_fd, (char *)&rs, sizeof(rs));
            }
        }
    }
    alarm(kHeartDetectTime);
}

//连接数据库，连接网络。InitNetWork中进行epoll_wait，并创建了线程池。
int TcpKernel::Open()
{
    //测试用，初始化一些房间
    list<UserInfo *> ls1;
    UserInfo *user1 = new UserInfo(55, 4, 123456);
    UserInfo *user3 = new UserInfo(57, 5, 123456);

    strcpy(user1->m_user_name, "c");
    strcpy(user3->m_user_name, "d");

    ls1.push_back(user1);
    ls1.push_back(user3);

    list<UserInfo *> ls2;
    UserInfo *user2 = new UserInfo(56, 3, 123455);
    strcpy(user2->m_user_name, "b");
    ls2.push_back(user2);
    //结束////////

    m_mapRoomIDToUserList[123456] = ls1;
    m_mapRoomIDToUserList[123455] = ls2;
    m_mapIDToUserInfo[3] = user2;
    m_mapIDToUserInfo[4] = user1;
    m_mapIDToUserInfo[5] = user3;

    InitRandom();
    m_sql = new CMysql;
    m_tcp = new TcpNet(this);
    m_tcp->SetpThis(m_tcp);
    pthread_mutex_init(&m_tcp->alock, NULL);
    pthread_mutex_init(&m_tcp->rlock, NULL);
    if (!m_sql->ConnectMysql((char *)"localhost", (char *)"root", (char *)"123", (char *)"user"))
    {
        printf("Conncet Mysql Failed...\n");
        return false;
    }
    else
    {
        printf("MySql Connect Success...\n");
    }
    if (!m_tcp->InitNetWork())
    {
        printf("InitNetWork Failed...\n");
        return false;
    }
    else
    {
        printf("Init TcpNet Success...\n");
    }

    //TODO：开启一个定时器
    setThis();
    sighandler_t handler = worker;

    signal(SIGALRM, handler);
    alarm(kHeartDetectTime);

    return true;
}

//每一个用户都关闭，然后断开网络连接和数据库
void TcpKernel::Close()
{
    for (auto ite = m_mapIDToUserInfo.begin(); ite != m_mapIDToUserInfo.end(); ++ite)
    {
        delete ite->second;
    }
    m_mapIDToUserInfo.clear();
    m_sql->DisConnect();
    m_tcp->UnInitNetWork();
}

void TcpKernel::DealData(int clientfd, char *szbuf, int nlen)
{
    PackType *pType = (PackType *)szbuf;
    int i = 0;
    while (1)
    {
        if (*pType == m_ProtocolMapEntries[i].m_type)
        {
            //函数指针，对应着m_ProtocolMapEntries里的函数。TODO：改成c++11的function。
            auto fun = m_ProtocolMapEntries[i].m_pfun;
            (this->*fun)(clientfd, szbuf, nlen);
        }
        else if (m_ProtocolMapEntries[i].m_type == 0 &&
                 m_ProtocolMapEntries[i].m_pfun == 0)
            return;
        ++i;
    }
    return;
}

void TcpKernel::HeartDetect(int clientfd, char *szbuf, int nlen)
{

	static int i = 0;
    StructHeartDetectResponse *rq = reinterpret_cast<StructHeartDetectResponse *>(szbuf);

    //人不在线
    if (m_mapIDToUserInfo.find(rq->m_user_id) == m_mapIDToUserInfo.end())
	{
		cout << "心跳检测无此用户！" << endl;
		return;
	}

    UserInfo *user = m_mapIDToUserInfo[rq->m_user_id];

	if (i % 5 == 0) 
	{
		cout << "user:" << user->m_user_name << " " << "heart!" << endl;		
		i = 0;
	}
	++i;

	user->m_state = 1;
}

void TcpKernel::InitRandom()
{
    //时间做随机种子
    struct timeval time;
    gettimeofday(&time, NULL);
    srand(time.tv_sec + time.tv_usec);
}

//创建房间请求
void TcpKernel::CreateRoomequest(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d CreateRoomequest\n", clientfd);
    //解析
    StructCreateRoomRequest *rq = reinterpret_cast<StructCreateRoomRequest *>(szbuf);

    //人不在线
    if (m_mapIDToUserInfo.find(rq->m_user_id) == m_mapIDToUserInfo.end())
	{
		cout << "创建房间无此用户！" << endl;	
        return;
	}

    UserInfo *user = m_mapIDToUserInfo[rq->m_user_id];

    //随机创建房间，重复的话重新开始
    int roomid = 0;
    do
    {
        roomid = rand() % 900000 + 100000;
    } while (roomid == 0 || (m_mapRoomIDToUserList.find(roomid) != m_mapRoomIDToUserList.end()));

    StructCreateRoomRespose rs;
    rs.m_room_id = roomid;
    rs.m_create_room_result = kCreateSuccess;
    rs.m_room_owner_id = rq->m_user_id;
    strcpy(rs.m_room_owner_name, user->m_user_name);

    //存储在map,创建者添加到list --> map
    list<UserInfo *> lst;
    lst.push_back(user);
    user->m_room_id = roomid;
    m_mapRoomIDToUserList[roomid] = lst; // 浅拷贝 -- > 拷贝构造
    cout << "创建房间，房间号是" << roomid << endl;
    //回复包,告诉客户端创建成功
    m_tcp->SendData(clientfd, (char *)&rs, sizeof(rs));
    //显示房间列表，只有房主一人
    StructRoomMemberResponse member_rs;
    member_rs.m_user_id = rq->m_user_id;
    strcpy(member_rs.m_sz_user, user->m_user_name);
    m_tcp->SendData(clientfd, (char *)&member_rs, sizeof(member_rs));
}

//加入房间请求
void TcpKernel::JoinRoomequest(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d JoinRoomequest\n", clientfd);
    //解析
    StructJoinRoomRequest *rq = reinterpret_cast<StructJoinRoomRequest *>(szbuf);
    StructJoinRoomResponse rs;

    //加入者的信息 不在线 退出->结果
    if (m_mapIDToUserInfo.find(rq->m_user_id) == m_mapIDToUserInfo.end())
    {
		cout << "加入者用户不存在！" << endl;
        return;
    }

    UserInfo *joiner = m_mapIDToUserInfo[rq->m_user_id];
    //加入房间 拿到list 查不到房间 结果失败
    if (m_mapRoomIDToUserList.find(rq->m_room_id) == m_mapRoomIDToUserList.end() && rq->m_room_id != 123456 && rq->m_room_id != 123455)
    {
        rs.m_join_room_result = kRoomNotExist;
        m_tcp->SendData(clientfd, (char *)&rs, sizeof(rs));
        return;
    }

    joiner->m_room_id = rq->m_room_id;
    //拿到当前房间列表
    list<UserInfo *> lst = m_mapRoomIDToUserList[rq->m_room_id];
    rs.m_join_room_result = kJoinSuccess;
    rs.m_room_id = rq->m_room_id;
    rs.m_join_id = rq->m_user_id;
    rs.m_room_owner_id = lst.front()->m_id;
    strcpy(rs.m_room_owner_name, lst.front()->m_user_name);
    strcpy(rs.m_join_name, joiner->m_user_name);
    //发送加入房间回复结果
    m_tcp->SendData(clientfd, (char *)&rs, sizeof(rs));

    //遍历list,对于加入者，把房间内的每一个用户信息发给他；对于房间用户，把加入者的信息分别发给他们。
    StructRoomMemberResponse joiner_rq;
    joiner_rq.m_user_id = rq->m_user_id;
    strcpy(joiner_rq.m_sz_user, joiner->m_user_name);

    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        UserInfo *inner = *it;

        //通知有用户加入请求
        StructRoomMemberResponse inner_rq;
        inner_rq.m_user_id = inner->m_id;
        strcpy(inner_rq.m_sz_user, inner->m_user_name);

        //发送加入者信息给房间每个人
        m_tcp->SendData(inner->m_fd, (char *)&joiner_rq, sizeof(joiner_rq));

        //发送房间内所有人的信息给加入者
        m_tcp->SendData(joiner->m_fd, (char *)&inner_rq, sizeof(inner_rq));
    }
    //加入者自身信息也要显示
    m_tcp->SendData(joiner->m_fd, (char *)&joiner_rq, sizeof(joiner_rq));

    //添加到list
    lst.push_back(joiner);
    //更新map
    m_mapRoomIDToUserList[rq->m_room_id] = lst;
}

//离开房间请求
void TcpKernel::QuitRoomequest(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d QuitRoomequest\n", clientfd);
    //解析
    StructQuitRoomRequest *rq = reinterpret_cast<StructQuitRoomRequest *>(szbuf);

    //找用户信息  map
    if (m_mapIDToUserInfo.find(rq->m_user_id) == m_mapIDToUserInfo.end()){
		cout << "退出房间的用户不存在！"  << endl;
		return;
	}

    UserInfo *leaver = m_mapIDToUserInfo[rq->m_user_id];

    //找房间信息 map
    if (m_mapRoomIDToUserList.find(rq->m_room_id) == m_mapRoomIDToUserList.end()){
		cout << "退出房间的用户所在房间不存在！"  << endl;
		return;
	}

    list<UserInfo *> lst = m_mapRoomIDToUserList[rq->m_room_id]; //找到  拿到list
    cout << "user:" << leaver->m_user_name << "离开了房间" << rq->m_room_id << endl;
    //leaver->m_room_id = 0;
    StructQuitRoomResponse rs;
    StructRoomMemberResponse member_rs;
    //告诉每一个非离开的人，清空用户列表
    StructRoomMemberHeaderResponse head_rs;

    //TODO:主持人退出的话，房间解散
    if (rq->m_user_id == lst.front()->m_id)
    {
        //遍历链表，让每一个用户都退出当前房间
        auto it = lst.begin();
        while (it != lst.end())
        {
            UserInfo *inner = *it;
            rs.m_user_id = inner->m_id;
            strcpy(rs.szUserName, inner->m_user_name);
            m_tcp->SendData(inner->m_fd, (char *)&rs, sizeof(rs));
            ++it;
        }
        //遍历完毕，清空链表，删除hash对应内容
        lst.clear();
        m_mapRoomIDToUserList.erase(rq->m_room_id);
        return;
    }

    rs.m_user_id = rq->m_user_id;
    strcpy(rs.szUserName, leaver->m_user_name);

    //遍历list，告诉每一个用户有一个用户离开了
    auto it = lst.begin();
    while (it != lst.end())
    {
        UserInfo *inner = *it;

        //发给每一个用户离开人的信息，其实就是更新用户列表
        if (inner->m_id != rq->m_user_id) //不需要发给自己
        {
            //需要先让每一个用户清空，然后再遍历链表发给用户。
            head_rs.m_user_id = inner->m_id;
            this->SendMsgToOnlineClient(inner->m_id, (char *)&head_rs, sizeof(head_rs));
            for (auto iter = lst.begin(); iter != lst.end(); ++iter)
            {
                if (rq->m_user_id != (*iter)->m_id) //退出房间的那个人不需要再列中中显示
                {
                    member_rs.m_user_id = (*iter)->m_id;
                    memset(member_rs.m_sz_user, 0, kMaxSize);
                    strcpy(member_rs.m_sz_user, (*iter)->m_user_name);
                    this->SendMsgToOnlineClient(inner->m_id, (char *)&member_rs, sizeof(member_rs));
                }
            }
            ++it;
        }
        else
        { //从list移除这个节点
            it = lst.erase(it);
            if (lst.empty())
                m_mapRoomIDToUserList.erase(rq->m_room_id);
        }
    }
    //更新map
    if (!lst.empty())
        m_mapRoomIDToUserList[rq->m_room_id] = lst;
    m_tcp->SendData(leaver->m_fd, (char *)&rs, sizeof(rs));
}

//下线请求
void TcpKernel::Offlineequest(int clientfd, char *szbuf, int nlen)
{
    StructOffLineRequest *rq = reinterpret_cast<StructOffLineRequest*>(szbuf);
	if (m_mapIDToUserInfo.find(rq->m_user_id) == m_mapIDToUserInfo.end()){
		cout << "下线请求的用户不存在！" << endl;
		return;
	}
	UserInfo* quiter = m_mapIDToUserInfo[rq->m_user_id];
	printf("clientfd:%d   user:%s  Offline\n", clientfd, quiter->m_user_name);
	m_mapIDToUserInfo.erase(quiter->m_id);
	delete quiter;
}

//刷新房间列表请求
void TcpKernel::RefreshRoomList(int clientfd, char *szbuf, int nlen)
{
    //解析
    StructRoomListRefreshHeaderRequest *rq = reinterpret_cast<StructRoomListRefreshHeaderRequest *>(szbuf);
    //找用户信息  map
    if (m_mapIDToUserInfo.find(rq->m_user_id) == m_mapIDToUserInfo.end()){
		cout << "刷新房间列表请求用户不存在！" << endl;
		return;
	}
	
    printf("clientfd:%d RefreshRoomListRequest\n", clientfd);
	SendRoomList(rq->m_user_id);
}

//转发给在线用户
void TcpKernel::SendMsgToOnlineClient(int id, char *szbuf, int nlen)
{
    if (m_mapIDToUserInfo.find(id) != m_mapIDToUserInfo.end())
    {
        m_tcp->SendData(m_mapIDToUserInfo[id]->m_fd, szbuf, nlen);
    } else {
		cout << "转发给在线用户不存在！" << endl;
	}
}

//注册
void TcpKernel::Registerequest(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d Registerequest\n", clientfd);

    StructRegisterRequest *rq = reinterpret_cast<StructRegisterRequest *>(szbuf);
    StructRegisterRespose rs;

    char sqlBuf[kSqlSize] = "";
    sprintf(sqlBuf, "select username from user where username= '%s';", rq->m_szUser);
    list<string> res_list;
    bool res = m_sql->SelectMysql(sqlBuf, 1, res_list);
    if (!res)
    {
        cout << "SelectMysql error: " << sqlBuf << endl;
        return;
    }
    //查表 查name
    if (res_list.size() > 0)
    {
        //有  不成功
        rs.m_register_result = kUserIdExist;
    }
    else
    { //没有 写表 成功
        char sqlBuf[kSqlSize] = "";
        sprintf(sqlBuf, "insert into user (username ,passwd) values('%s','%s');",
                rq->m_szUser, rq->m_szPassword);
        m_sql->UpdataMysql(sqlBuf);
        rs.m_register_result = kRegisterSucess;
    }
    m_tcp->SendData(clientfd, (char *)&rs, sizeof(rs));
}

//登录
void TcpKernel::Loginequest(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d Loginequest\n", clientfd);

    StructLoginRequest *rq = reinterpret_cast<StructLoginRequest *>(szbuf);
    StructLoginRespose rs;

    char buf[kSqlSize] = "";
    sprintf(buf, "select passwd ,id from user where username = '%s';", rq->m_user_name);
	//printf("sql: %s", buf);
    list<string> res_list;
    //res_list存着密码和id
    bool res = m_sql->SelectMysql(buf, 2, res_list);
    if (!res)
    {
        cout << "SelectMysql error:" << buf << endl;
    }

//	for (auto it = res_list.begin(); it != res_list.end(); ++it) {
//		cout << (*it) << endl;
//	}

    if (res_list.size() > 0)
    {
        //先比较头部的密码
        if (strcmp(res_list.front().c_str(), rq->m_user_passwd) == 0)
        {
            rs.m_login_result = kLoginSucess;
            res_list.pop_front();
            //再获取id
            rs.m_user_id = atoi(res_list.front().c_str());
            strcpy(rs.m_user_name, rq->m_user_name);

            if (m_mapIDToUserInfo.find(rs.m_user_id) == m_mapIDToUserInfo.end())
            { //没找到
                //TODO:改成智能指针，防止内存泄露
                UserInfo *info = new UserInfo;
                info->m_fd = clientfd;
                info->m_id = rs.m_user_id;
                strcpy(info->m_user_name, rq->m_user_name);
                info->m_state = 1; //TODO:看状态定房主等操作

                m_mapIDToUserInfo[rs.m_user_id] = info;
                m_tcp->SendData(clientfd, (char *)&rs, sizeof(rs));
            }
            else
            { //找到，再次上线，拒绝。
                rs.m_login_result = kUserOnLine;
                m_tcp->SendData(clientfd, (char *)&rs, sizeof(rs));
                return;
            }
            //TODO:把当前房间列表发给用户。
            cout << "发放房间列表！" << endl;
            getUserInfoFromSql(rs.m_user_id);
            SendRoomList(rs.m_user_id);
            return;
        }
        else
        {
            rs.m_login_result = kPasswdError;
        }
    }
    else
    {
        rs.m_login_result = kUserIdNotExist;
    }
    m_tcp->SendData(clientfd, (char *)&rs, sizeof(rs));
}

//从数据库获取个人信息
UserInfo *TcpKernel::getUserInfoFromSql(int id)
{
    if (m_mapIDToUserInfo.find(id) == m_mapIDToUserInfo.end())
        return NULL;
    UserInfo *info = m_mapIDToUserInfo[id];
    //从数据库里面取 更新info 再返回
    char sqlbuf[kSqlSize] = "";
    sprintf(sqlbuf, "select username from user where id = %d", id);
    list<string> res;
    m_sql->SelectMysql(sqlbuf, 1, res);
    if (res.size() > 0)
    {
        strcpy(info->m_user_name, res.front().c_str());
        res.pop_front();
    }
    return info;
}

//发送房间列表给用户
void TcpKernel::SendRoomList(int id)
{
    //先找该用户
    if (m_mapIDToUserInfo.find(id) == m_mapIDToUserInfo.end())
	{
		cout << "查无此人!" << endl;
		return;
	}
	
    cout << "发送房间列表给用户" << endl;

    //写用户信息 loginer  用户信息请求loginequest
    UserInfo *loginer = m_mapIDToUserInfo[id];

    //因为房间列表信息是循环发送的，这里先发一个头部信息，告诉用户要更新了，让用户先清空自己的用户列表，然后再一个一个添加。
    StructRoomListRefreshHeaderResponse rs_head;
    m_tcp->SendData(loginer->m_fd, (char *)&rs_head, sizeof(rs_head));

    StructRoomListResponse rs;
    //先判断房间列表是否存在，不然会报错
    if (!m_mapRoomIDToUserList.empty())
    {
        for (auto it = m_mapRoomIDToUserList.begin(); it != m_mapRoomIDToUserList.end(); ++it)
		{
			
            rs.m_room_num = it->second.front()->m_room_id;
			//cout << "当前房间号有：" << rs.m_room_num << endl;
            strcpy(rs.m_owner_name, it->second.front()->m_user_name);
            //向登录的人发送房间列表的信息，rq就是结构体类型
            m_tcp->SendData(loginer->m_fd, (char *)&rs, sizeof(rs));
        }
    }
}

//发送用户列表给用户
void TcpKernel::SendUserList(int clientfd, char *szbuf, int nlen)
{
    StructRoomMemberRequset *rq = reinterpret_cast<StructRoomMemberRequset *>(szbuf);
    StructRoomMemberHeaderResponse head_rs;
    head_rs.m_user_id = rq->m_user_id;
    //加入者的信息 不在线 退出->结果
    if (m_mapIDToUserInfo.find(rq->m_user_id) == m_mapIDToUserInfo.end())
    {
		cout << "发送用户列表的用户不在线！" << endl;
        return;
    }
    this->SendMsgToOnlineClient(head_rs.m_user_id, (char *)&head_rs, sizeof(head_rs));
    UserInfo *user = m_mapIDToUserInfo[rq->m_user_id];
    //拿到当前房间列表
    list<UserInfo *> lst = m_mapRoomIDToUserList[user->m_room_id];

    //遍历list,对于加入者，把房间内的每一个用户信息发给他；对于房间用户，把加入者的信息分别发给他们。
    StructRoomMemberResponse joiner_rq;
    joiner_rq.m_user_id = rq->m_user_id;
    strcpy(joiner_rq.m_sz_user, user->m_user_name);
    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        UserInfo *inner = *it;

        //通知有用户加入请求
        StructRoomMemberResponse inner_rq;
        inner_rq.m_user_id = inner->m_id;
        strcpy(inner_rq.m_sz_user, inner->m_user_name);

        //发送房间内所有人的信息给加入者
        m_tcp->SendData(user->m_fd, (char *)&inner_rq, sizeof(inner_rq));
    }
}

//转让房主回复
void TcpKernel::TransferUserRequset(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d TransferUserRequset\n", clientfd);
    StructTransferUserRequest *rq = reinterpret_cast<StructTransferUserRequest *>(szbuf);

    //自己不能转让给自己
    int new_owner_id = rq->transfer_user_id;
    UserInfo *old_owner = m_mapIDToUserInfo[rq->m_user_id];
    UserInfo *new_owner = m_mapIDToUserInfo[rq->transfer_user_id];

    //遍历链表，把房主放在最前面
    auto lst = m_mapRoomIDToUserList[old_owner->m_room_id];

    if (old_owner != lst.front())
    {
        //TODO:请求人原来一定在链表头部
        printf("请求人不是房主！\n");
        return;
    }
    for (auto it = lst.begin(); it != lst.end();)
    {
        if ((*it) == new_owner)
        {
            it = lst.erase(it);
        }
        ++it;
    }
    lst.push_front(new_owner);

    StructTransferUserResponse rs;
    rs.m_user_id = old_owner->m_id;
    rs.transfer_user_id = new_owner_id;
    strcpy(rs.transferUserName, new_owner->m_user_name);
    //通知所有人房主已结更改,
    m_mapRoomIDToUserList[old_owner->m_room_id] = lst;
    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        //TODO:更新用户列表，暂时因为房间上会显示房主，所以不做了
        m_tcp->SendData((*it)->m_fd, (char *)&rs, sizeof(rs));
    }
}

//静音用户处理
void TcpKernel::MuteOneUserRequset(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d MuteOneUserRequset\n", clientfd);

    StructMuteUserRequest *rq = reinterpret_cast<StructMuteUserRequest *>(szbuf);
    UserInfo *muted_user = m_mapIDToUserInfo[rq->mute_user_id];
    muted_user->is_muted = true;

    //遍历链表，通知所有人，xx被静音。
    auto lst = m_mapRoomIDToUserList[muted_user->m_room_id];

    StructMuteUserResponse rs;
    rs.mute_user_id = rq->mute_user_id;
    strcpy(rs.muteUserName, m_mapIDToUserInfo[rq->mute_user_id]->m_user_name);

    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        m_tcp->SendData((*it)->m_fd, (char *)&rs, sizeof(rs));
    }
}

void TcpKernel::UnMuteRequset(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d UnMuteRequset\n", clientfd);

    StructUnMuteRequest *rq = reinterpret_cast<StructUnMuteRequest *>(szbuf);
    UserInfo *user = m_mapIDToUserInfo[rq->m_user_id];
    user->is_muted = false;

    //遍历链表，通知所有人，xx解除静音。
    auto lst = m_mapRoomIDToUserList[user->m_room_id];

    StructUnMuteResponse rs;
    rs.m_user_id = user->m_id;
    strcpy(rs.szUserName, user->m_user_name);

    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        m_tcp->SendData((*it)->m_fd, (char *)&rs, sizeof(rs));
    }
}

//踢出用户回复
void TcpKernel::KickOutOfUserRequset(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d KickOutOfUserRequset\n", clientfd);

    StructKickOutOfUserRequest *rq = reinterpret_cast<StructKickOutOfUserRequest *>(szbuf);
    int kicked_user_id = rq->kick_user_id;
    UserInfo *kicked_user = m_mapIDToUserInfo[kicked_user_id];

    //遍历链表，把该用户踢出
    auto lst = m_mapRoomIDToUserList[kicked_user->m_room_id];
    if (kicked_user_id == rq->m_user_id)
    {
        printf("不能房主踢房主！\n");
        return;
    }

    StructKickOutOfUserResponse rs;
    rs.m_user_id = rq->m_user_id;
    rs.kick_user_id = kicked_user->m_id;
    strcpy(rs.kickrUserName, kicked_user->m_user_name);

    //告诉每一个非离开的人，清空用户列表
    StructRoomMemberHeaderResponse head_rs;
    StructRoomMemberResponse member_rs;
    //遍历list，告诉每一个用户有一个用户离开了
    auto it = lst.begin();
    while (it != lst.end())
    {
        UserInfo *inner = *it;

        //发给每一个用户离开人的信息，其实就是更新用户列表
        if (inner->m_id != kicked_user->m_id) //不需要发给被题用户
        {
            //需要先让每一个用户清空，然后再遍历链表发给用户。
            head_rs.m_user_id = inner->m_id;

            //先发一个头部，清空用户列表
            this->SendMsgToOnlineClient(inner->m_id, (char *)&head_rs, sizeof(head_rs));
            //每个用户发一个踢人回复
            this->SendMsgToOnlineClient(inner->m_id, (char *)&rs, sizeof(rs));
            for (auto iter = lst.begin(); iter != lst.end(); ++iter)
            {
                if (kicked_user != (*iter)) //退出房间的那个人不需要再列中中显示
                {
                    member_rs.m_user_id = (*iter)->m_id;
                    memset(member_rs.m_sz_user, 0, kMaxSize);
                    strcpy(member_rs.m_sz_user, (*iter)->m_user_name);
                    this->SendMsgToOnlineClient(inner->m_id, (char *)&member_rs, sizeof(member_rs));
                }
            }
            ++it;
        }
        else
        {
            //先发一个头部，清空用户列表
            this->SendMsgToOnlineClient(inner->m_id, (char *)&head_rs, sizeof(head_rs));
            //也告诉被题的人
            this->SendMsgToOnlineClient(inner->m_id, (char *)&rs, sizeof(rs));
            //从list移除这个节点
            it = lst.erase(it);
            if (lst.empty())
                m_mapRoomIDToUserList.erase(kicked_user_id);
        }
    }
    //更新map
    if (!lst.empty())
        m_mapRoomIDToUserList[kicked_user->m_room_id] = lst;
}
