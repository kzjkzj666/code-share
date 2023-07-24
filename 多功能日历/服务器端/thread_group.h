#ifndef THREAD_GROUP_H
#define THREAD_GROUP_H

#include <QThread>
#include<QJsonArray>
#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QDate>
#include<QTime>
#include<QMap>
class thread_group : public QThread
{
    Q_OBJECT
public:
    explicit thread_group(QThread *parent = nullptr);
    void new_member(QTcpSocket * sock,QString id);
    QHash<QTcpSocket*,QString>map;
    QJsonArray arr;
    QString str;
signals:
    void id_all(  QHash<QTcpSocket*,QString>map);
    void new_message(QString text,QString id);
protected:
    void run();
};

#endif // THREAD_GROUP_H
