#include "send.h"
#include<QJsonValue>
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include<QDir>
#include<QJsonArray>
#include<QTimer>
send::send()
{
    sock=new QTcpSocket;
    const int port=PORT_FILE;
    QString ip=IP;
    sock->connectToHost(QHostAddress(ip), port);
    connect(sock,&QIODevice::readyRead,this,[=]
            {
      hear();
    });
}
void send:: sendfile(QByteArray data)
{
    this->data=data;
    QTimer* t_time = new QTimer(this);
    t_time->setSingleShot(1);
    t_time->start(500);//设置时间间隔为1秒
    connect(t_time, &QTimer::timeout,[=]
            {
               sock->write(data);
            }
            );

}
void send::hear()
{
    QByteArray data=sock->readAll();
    QJsonDocument doc=QJsonDocument::fromJson(data);
    QJsonObject obj=doc.object();
    if(obj.value("type")=="0"||obj.value("type")=="5")//注册/创建团队状态返回
    {
        if(obj.value("status")=="1")
        //服务器返回了注册成功的信息
        {
            emit success();
        }
        if(obj.value("status")=="0")
        {

          emit fail();
        }
    }
    else if(obj.value("type")=="1"||obj.value("type")=="4")//登录/加入团队状态返回
    {

        if(obj.value("status")=="1")//1--成功，0--密码错误，2--用户不存在
        {
          emit log_success();
        }
        else if(obj.value("status")=="0")
        {
          emit password_error();
        }
         else if(obj.value("status")=="2")
        {
          emit id_not_exist();
        }
        else if(obj.value("status")=="3")
        {
          emit joined();
        }
    }
    else if(obj.value("type")=="6"&&obj.value("status")=="1")//添加日程状态返回
    {
        emit get_message();
    }
    else if(obj.value("type")=="2"||obj.value("type")=="12")
    {
        QFile file_1(QDir::currentPath()+"/"+ obj.value("id").toString()+"_plan.json");
        file_1.open(QIODeviceBase::WriteOnly);
        file_1.write(QJsonDocument(obj.value("myplan").toArray()).toJson());
        QFile file_2(QDir::currentPath()+"/"+ "user_plan_sum.json");
         file_2.open(QIODeviceBase::WriteOnly);
        file_2.write(QJsonDocument(obj.value("otherplan").toArray()).toJson());
         QFile file_3(QDir::currentPath()+"/"+obj.value("id").toString()+"_teamlist.json");
        file_3.open(QIODeviceBase::WriteOnly);
        file_3.write(QJsonDocument(obj.value(obj.value("id").toString()+"_teamlist").toArray()).toJson());
        if(obj.value("name").toString()!="")
        {
        QFile file_4(QDir::currentPath()+"/"+ obj.value("name").toString()+"_teamplan.json");
        file_4.open(QIODeviceBase::WriteOnly);
        file_4.write(QJsonDocument(obj.value("teamplan").toArray()).toJson());
        file_4.close();
        }
        file_1.close();
        file_2.close();
        file_3.close();
        if( obj.value("type")=="2")
        emit get_message();
    }
}
send::~send()
{
    this->sock->close();
}

