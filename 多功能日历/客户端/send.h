#ifndef SEND_H
#define SEND_H
#include<QTcpSocket>
#include<QByteArray>
#include<data.h>
class send:public QObject
{
    Q_OBJECT;
public:
    send();
    ~send();
    void sendfile(QByteArray);
    void hear();
private:
    QByteArray data;
    QTcpSocket* sock;
signals:
    void get_message();
    void success();
    void fail();
    void id_not_exist();
    void password_error();
    void log_success();
    void joined();

};
#endif // SEND_H
