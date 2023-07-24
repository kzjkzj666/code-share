#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include<QTcpSocket>
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include<QDir>
#include<QJsonArray>

class thread : public QThread
{
    Q_OBJECT
public:
    explicit thread(QTcpSocket*,QObject *parent = nullptr);
    QTcpSocket * tcp;
signals:
    void new_team_add(QString name);
protected:
    void run();
};
#endif // THREAD_H
