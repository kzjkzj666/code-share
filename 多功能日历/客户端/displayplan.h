#ifndef DISPLAYPLAN_H
#define DISPLAYPLAN_H
#include <QWidget>
#include<vector>
#include<QString>
#include<QFile>
#include<string.h>
#include<QDate>
#include<QLabel>
#include<QPushButton>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonValue>
using namespace  std;
struct plan_s
{
    QString id,date,time,privacy,title,content,place,creat_time,endtime;
};
namespace Ui {
class displayplan;
}

class displayplan : public QWidget
{
    Q_OBJECT
private:
    vector<plan_s>arr_plan;
    vector<QLabel*>arr_label;
    vector<QPushButton*>arr_btn;
    vector<QPushButton*>arr_del;
    QHash<QPushButton*,QString>hash;
    QString date;
    QString m_id;
public:
    explicit displayplan(QString,QWidget *parent = nullptr);
    ~displayplan();
    void refresh();
    void display();
    void clear();
    void clear_arr_plan();
    void closeEvent(QCloseEvent *);
signals:
    void back();
    void should_dis();
    void link();
private:
     Ui::displayplan *ui;
};

#endif // DISPLAYPLAN_H
