#ifndef SEARCH_H
#define SEARCH_H
#include <QWidget>
#include<QFile>
#include<QJsonValue>
#include<QJsonObject>
#include<QJsonDocument>
#include<QFile>
#include<QDir>
#include<QJsonArray>
namespace Ui {
class search;
}
struct m_plan
{
    QDate date;
    QTime time;
    QTime endtime;
    QString id;
};
struct m_plan_t
{
    QDate date;
    QTime time;
    QTime endtime;
    QString id,title,place,content;
};
class search : public QWidget
{
    Q_OBJECT
private:
    QString m_id;
    QString name;
    std::vector<m_plan>arr;
    std::vector<m_plan_t>arr_team;
    QJsonArray member;
public:
    explicit search(QString,QString,QWidget *parent = nullptr);
    ~search();
    void closeEvent(QCloseEvent *);
    void load();
signals:
    void back();
private:
    Ui::search *ui;
};

#endif // SEARCH_H
