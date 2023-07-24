#ifndef WORK_H
#define WORK_H
#include <QWidget>
#include<QDate>
#include<QTime>
#include<QJsonValue>
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include<QDir>
#include<QJsonArray>
namespace Ui {
class work;
}
struct plan
{
    QDate date;
    QTime time;
    QTime endtime;
    QString id;
};
struct plan_t
{
    QDate date;
    QTime time;
    QTime endtime;
    QString id,title,place,content;
};
class work : public QWidget
{
    Q_OBJECT
private:
     QString name,m_id;
    QDate date;
    std::vector<plan>arr;
    std::vector<plan_t>arr_team;
    QJsonArray member;
    int flag_link;
      Ui::work *ui;
public:
    explicit work(QString,QString , QWidget *parent = nullptr);
    ~work();
    void checkplan();
    void load();
     void closeEvent(QCloseEvent *);
signals:
    void back();
};

#endif // WORK_H
