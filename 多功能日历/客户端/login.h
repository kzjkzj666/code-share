#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include"mainwindow.h"
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include"send.h"
#include<QTimer>
#include<QMessageBox>
#include<QEventLoop>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

private:
    send * m_send;
    MainWindow* w;
    int flag;
public:
    explicit login(QWidget *parent = nullptr);
    void log();
    void id_not_exist();
    void password_error();
    ~login();
signals:
    void log_success();
protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器

private:
    Ui::login *ui;
};

#endif // LOGIN_H
