#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QJsonArray>
#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QDate>
#include<QTime>
#include<QDir>
#include<QMap>
#include"thread_group.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTcpServer * m_server;
    QTcpServer * server_chat;
    QHash<QString,thread_group*>group;
signals:
    void id_all(  QHash<QTcpSocket*,QString>map);
    void new_message(QString text,QString id);
    void new_group_connect(QTcpSocket * sock,QString id);
private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
