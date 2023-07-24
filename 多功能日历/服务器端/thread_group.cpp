#include "thread_group.h"
#include"thread_chat.h"
thread_group::thread_group(QThread *parent)
    : QThread{parent}
{

}
void thread_group::new_member(QTcpSocket * sock,QString id)
{
    thread_chat* trd=new thread_chat(sock);
    map.insert(sock,id);
    trd->start();
    connect(this,&thread_group::id_all,trd,&thread_chat::receive_from_main);
    connect(this,&thread_group::new_message,trd,&thread_chat::SendMessageToClient);
    connect(trd,&thread_chat::new_message,[=](QString s,QString id)
            {
        qDebug()<<s<<"  "<<id;
                emit new_message(s,id);
            });
    connect(sock,&QTcpSocket::disconnected,[=]
            {
                map.remove(sock);
                emit id_all(map);
                delete trd;
            });
    emit id_all(map);
}
void::thread_group::run()
{


}
