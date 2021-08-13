/*
 * @Author: FengYanBin
 * @Date: 2021-08-09 11:19:42
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-11 09:36:43
 * @Description: file content
 * @FilePath: \sql\net_api\udp_net.cpp
 */
#include "udp_net.h"

UdpNet::UdpNet() : port_(PORT)
{
    InitNet();
}

bool UdpNet::InitNet()
{
    m_udpSocket = new QUdpSocket();
    qDebug() << "udp server port:" << port_ << " "
             << "server ip:" << SERVERIP << endl;
    m_udpSocket->bind(QHostAddress::Any, port_);
    m_destaddr.setAddress(SERVERIP);

    qDebug() << "udp连接成功！" << endl;
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpNet::onUdpReadyRead);
}

void UdpNet::CloseNet()
{
    if (m_udpSocket)
        m_udpSocket->close();
}

bool UdpNet::SendData(QByteArray data, QString IP)
{
    if (m_udpSocket->writeDatagram(data, data.length(), QHostAddress(IP), port_))
    {
        return true;
    }
    return false;
}

bool UdpNet::SendBroadCast(QByteArray data)
{
    if (m_udpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, port_))
        return true;
    return false;
}

void UdpNet::onUdpReadyRead()
{
    //qDebug() << "UdpNet::onUdpReadyRead()";
    while (m_udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(int(m_udpSocket->pendingDatagramSize()));
        QHostAddress host;
        m_udpSocket->readDatagram(datagram.data(), datagram.size(), &host);
        qDebug() << "recvContent = " << datagram << endl;
    }
}

void UdpNet::run()
{
    qDebug() << "UdpNet::run()";
    while (!this->isInterruptionRequested())
    {
        QThread::sleep(1);
    }
}
