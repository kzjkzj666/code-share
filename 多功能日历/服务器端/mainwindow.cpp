#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thread.h"
#include<thread_chat.h>
#include<thread_group.h>

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
