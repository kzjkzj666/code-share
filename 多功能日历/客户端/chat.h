#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include"data.h"
#include<QJsonValue>
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include<QDir>
#include<QJsonArray>
#include<QTcpSocket>
#include<QByteArray>
#include<QKeyEvent>
namespace Ui {
class chat;
}

class chat : public QWidget
{
    Q_OBJECT
private:
    QString id;
      QTcpSocket * sock;
     QJsonArray member;
public:
    explicit chat(QString id,QString name,QJsonArray member,QWidget *parent = nullptr);
    void hear();
    ~chat();
    void send();
     void closeEvent(QCloseEvent *);
signals:
    void back();
protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器

private:
    Ui::chat *ui;
};

#endif // CHAT_H
