#include "thread_chat.h"

thread_chat::thread_chat(QTcpSocket* t ,QObject *parent)
    : QThread{parent}
{
   this->tcp=t;
}
void thread_chat::run()
{
    connect(tcp,&QIODevice::readyRead,[=]
            {
                QJsonObject obj=QJsonDocument::fromJson(tcp->readAll()).object();
                if(obj.value("type")=="2")
                    {
                    //消息
                    emit new_message(obj.value("text").toString(),obj.value("id").toString());
                }
            });
}
void thread_chat::receive_from_main(QHash<QTcpSocket*,QString>map)
{

    QStringList allvalues= map.values();
    arr=QJsonArray::fromStringList(allvalues);
    QJsonObject obj;
    obj.insert("type","1");//成员列表
    obj.insert("arr",arr);
    tcp->write(QJsonDocument(obj).toJson());
}
//开始为所有群聊都开辟一个线程，之后新的连接发起时，先单独使用一个线程处理，得到属于哪个群之后，再把这个传过去，之后有哪个群的子线程来维护
void thread_chat::SendMessageToClient(QString text,QString id)
{
    QJsonObject obj;
    obj.insert("type","2");
    obj.insert("text",text);
    obj.insert("id",id);
    tcp->write(QJsonDocument(obj).toJson());
}

