#include "mainwindow.h"
#include <QApplication>




//thread:
QThread *gUdpThd;
QThread *gTcpThd;


//TCP command
TcpCommand *gTcp;

//UDP receive
UdpReceive *gUdp;

//Package the socket source data,
//this function class is in Udp thread.
Package *gPkg;



Q_DECLARE_METATYPE(QAbstractSocket::SocketState)
Q_DECLARE_METATYPE(QAbstractSocket::SocketError)


int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //================ tcp socket =============================
    //tcp socket and command
    gTcp = new TcpCommand(0);
    gTcpThd = new QThread();

    //window - tcp freq
    QObject::connect(&w,SIGNAL(signCurFreq(quint32)),gTcp,SLOT(tcpFreq(quint32)));
    //window - tcp IP
    QObject::connect(&w,SIGNAL(signCurIp(QString)),gTcp,SLOT(tcpIp(QString)));
    //window - tcp port
    QObject::connect(&w,SIGNAL(signCurTcpPort(quint32)),gTcp,SLOT(tcpPort(quint32)));
    //window - tcp连接
    QObject::connect(&w,SIGNAL(signTcpCreate()),gTcp,SLOT(tcpCreate()));
    //window - tcp启动
    QObject::connect(&w,SIGNAL(signTcpStart()),gTcp,SLOT(tcpCmdStart()));
    //window - tcp回传
    QObject::connect(&w,SIGNAL(signTcpBack()),gTcp,SLOT(tcpCmdBack()));
    //window - tcp停止
    QObject::connect(&w,SIGNAL(signTcpStop()),gTcp,SLOT(tcpCmdStop()));
    //window - tcp关闭
    QObject::connect(&w,SIGNAL(signTcpClose()),gTcp,SLOT(tcpClose()));

    //window - tcp socketState
    qRegisterMetaType<QAbstractSocket::SocketState>("SocketState");
    QObject::connect(gTcp,SIGNAL(stateChanged(QAbstractSocket::SocketState)),&w,SLOT(slotStateChange(QAbstractSocket::SocketState)));

    //window - tcp errorString
    qRegisterMetaType <QAbstractSocket::SocketError>("SocketError");
    QObject::connect(gTcp,SIGNAL(error(QAbstractSocket::SocketError)),&w,SLOT(slotErrorString(QAbstractSocket::SocketError)));

    gTcp->moveToThread(gTcpThd);
    gTcpThd->start();

    //=============== udp socket =====================
    //udp receive and socket
    gUdp = new UdpReceive();
    gPkg = new Package();

    gUdpThd = new QThread();

    qRegisterMetaType <QPackage>("QPackage");
    qRegisterMetaType <QFrameData>("QFrameData");

    //window - udp port
    QObject::connect(&w,SIGNAL(signCurUdpPort(quint32)),gUdp,SLOT(udpPort(quint32)));
    //window - udp freq
    QObject::connect(&w,SIGNAL(signCurFreq(quint32)),gUdp,SLOT(udpFreq(quint32)));
    //window - udp open
    QObject::connect(&w,SIGNAL(signUdpOpen()),gUdp,SLOT(udpOpen()));
    //window - udp close
    QObject::connect(&w,SIGNAL(signUdpClose()),gUdp,SLOT(udpClose()));

    //udp - package: srcdata --> standard data
    QObject::connect(gUdp,SIGNAL(signUdpSrcData(QByteArray)),gPkg,SLOT(slotPkgRevSrcData(QByteArray)));




    gUdp->moveToThread(gUdpThd);
    gPkg->moveToThread(gUdpThd);
    gUdpThd->start();




    return a.exec();
}
