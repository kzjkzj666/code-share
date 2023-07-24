#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPainter>
#include<QTime>
#include<QFile>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonValue>
#include<QMessageBox>
#include<QTableWidgetItem>
#include"send.h"
#include<QDate>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    int flag;
    QDate date;
    QDate date_now;
    QString city;
    QString city_id;
    int m_i,m_j;
    QHash<QDate,QTableWidgetItem*>hash;
    QHash<QTableWidgetItem*,QString>inf,inf2;
      QTimer *t_time ;
     QString m_id;
public:
    MainWindow(QString,QWidget *parent = nullptr);
    ~MainWindow();
    void link();
    void link2();
    void closeEvent(QCloseEvent *);
    void weather();
    void hourweather();
    void showcalendar();
signals:
    void back();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
