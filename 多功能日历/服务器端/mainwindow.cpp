#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thread.h"
#include<thread_chat.h>
#include<thread_group.h>
//注册1，登录2，创建团队3，加入团队4，同步日程5，退出团队6，发布团队日程7，发群聊消息8
//服务器需要维护的信息：总的用户列表，每个用户的id，密码;团队日程；每个用户的日程，已有的团队信息（一个总文档和各个分文档）
//注册时，客户端发送用户名、密码，服务器端把这个用户加入已有的用户列表中，完成后告诉客户端注册成功，客户端接收后，弹窗显示
//登录时，客户端向服务器发送正在登录的用户名和密码，服务器在他维护的用户列表中查找，如果没有匹配的，返回用户不存在；
//否则登陆成功，向客户端发送已有的团队日程和该用户的存档日程（如果有），客户端接收这两个文档；服务器向客户端发送团队列表
//客户端创建个人日程操作，创建后先产生本地文件，然后把本地文件发送并同步给服务器。
//客户端创建团队，这个人默认为群主，并设置密码，一并发给服务器，服务器更新团队列表，客户端同步更新；
//用户申请加入团队，首先输入团队id,查找是否存在，存在则进入加入页面，服务器校验密码正确后，把这个用户加入团队，更新团队详细文档
//群主创建团队的指定日程，发给服务器，服务器更新
//文件以文本文档传输，格式为json
//服务器启动后进入监听状态，如果有某个客户端发起连接，就为这个客户端单独开辟一个线程，在这个子线程中对上述操作进行处理，连接断开后或退出登录，关闭线程，释放
//资源，客户端不使用多线程。

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server=new QTcpServer(this);
    server_chat=new QTcpServer(this);
    ui->close->setDisabled(1);
    connect(ui->btn,&QPushButton::clicked,this,[=]
            {
             QString time=QDate::currentDate().toString("yyyy年MM月dd日 ")+QTime::currentTime().toString("hh:mm:ss ");
                ui->textBrowser->append(time+"服务器启动成功");
                ui->btn->setDisabled(1);
                m_server->listen(QHostAddress::Any,10085);
                server_chat->listen(QHostAddress::Any,10087);
                ui->close->setEnabled(1);
        });
    connect(ui->close,&QPushButton::clicked,this,[=]
            {
        m_server->close();
        server_chat->close();
        QString time=QDate::currentDate().toString("yyyy年MM月dd日 ")+QTime::currentTime().toString("hh:mm:ss ");
        ui->textBrowser->append(time+"服务器关闭");
        ui->btn->setEnabled(1);
        ui->close->setDisabled(1);
            });
    connect(m_server,&QTcpServer::newConnection,this,[=]
            {
       QTcpSocket * tcp=m_server->nextPendingConnection();
       QString ip =QHostAddress( tcp->peerAddress().toIPv4Address()).toString();
        QString time=QDate::currentDate().toString("yyyy年MM月dd日 ")+QTime::currentTime().toString("hh:mm:ss ");
        ui->textBrowser->append(time+"新连接，IP "+ip);
        class thread * trd=new class thread(tcp);
        trd->start();
        connect(trd,&thread::new_team_add,[=](QString name)
                {
            thread_group * trd2=new thread_group;
            group.insert(name,trd2);
            trd2->start();
        });
        connect(tcp,&QTcpSocket::disconnected,[=]
                {
            delete trd;
             QString time=QDate::currentDate().toString("yyyy年MM月dd日 ")+QTime::currentTime().toString("hh:mm:ss ");
            ui->textBrowser->append(time+"连接断开，IP "+ip);
        });
            });
    QFile file1(QDir::currentPath()+"/teamlist.json");
    file1.open(QIODeviceBase::ReadOnly);
    QJsonArray arr=QJsonDocument::fromJson(file1.readAll()).array();
    file1.close();
    for(int i=0;i<arr.size();i++)
    {
        QJsonObject obj=arr[i].toObject();
        thread_group * trd=new thread_group;
        group.insert(obj.value("name").toString(),trd);
        trd->start();
    }
    connect(server_chat,&QTcpServer::newConnection,this,[=]
            {
                QTcpSocket * tcp=server_chat->nextPendingConnection();
                QString ip =QHostAddress( tcp->peerAddress().toIPv4Address()).toString();
                QString time=QDate::currentDate().toString("yyyy年MM月dd日 ")+QTime::currentTime().toString("hh:mm:ss ");
                ui->textBrowser->append(time+"新连接(chat)，IP "+ip);
                int x=0;
                connect(tcp,&QIODevice::readyRead,[=]
                        () mutable{
                    if(x==0)
                    {
                            x=1;
                            QJsonObject obj=QJsonDocument::fromJson(tcp->readAll()).object();
                            if(obj.value("type")=="0")//群聊
                            {
                                QString s=obj.value("name").toString();
                                thread_group* t=group.value(s);
                                connect(this,&MainWindow::new_group_connect,t,&thread_group::new_member);
                                emit new_group_connect(tcp,obj.value("id").toString());
                                disconnect(this,&MainWindow::new_group_connect,t,&thread_group::new_member);
                            }
                }
                });
        connect(tcp,&QTcpSocket::disconnected,[=]
                        {
                    QString time=QDate::currentDate().toString("yyyy年MM月dd日 ")+QTime::currentTime().toString("hh:mm:ss ");
                    ui->textBrowser->append(time+"连接断开(chat)，IP "+ip);
                });
    });
}
MainWindow::~MainWindow()
{
    delete ui;
}
