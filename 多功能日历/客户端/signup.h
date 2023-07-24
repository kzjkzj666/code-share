#ifndef SIGNUP_H
#define SIGNUP_H
#include <QWidget>
#include <QString>
#include <QPaintEvent>
#include <QMouseEvent>
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include"send.h"
#include<QTimer>

namespace Ui {
class signup;
}

class signup : public QWidget
{
    Q_OBJECT
private:
    QString vercode;
    int flag;
    send* m_send;
public:
    void generate();
    void paintEvent(QPaintEvent *);
    explicit signup(QWidget *parent = nullptr);
    void regs();//注册函数
    void closeEvent(QCloseEvent *);
    ~signup();
signals:
    void back();

private:
    Ui::signup *ui;
};

#endif // SIGNUP_H


