#ifndef THREAD_CHAT_H
#define THREAD_CHAT_H

#include <QThread>
#include<QTcpSocket>
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include<QDir>
#include<QJsonArray>


class thread_chat : public QThread
{
    Q_OBJECT
public:
    explicit thread_chat(QTcpSocket*,QObject *parent = nullptr);
    QTcpSocket * tcp;
    void receive_from_main( QHash<QTcpSocket*,QString>map);
    void SendMessageToClient(QString text,QString id);
    QJsonArray arr;
signals:
   void new_id(QString m_id);
   void new_message(QString text,QString id);
protected:
    void run();




};

#endif // THREAD_CHAT_H
