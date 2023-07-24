#ifndef ADDPLAN_H
#define ADDPLAN_H

#include <QDialog>
#include<QDate>
#include<QTime>
#include<QFile>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonValue>
#include<QMessageBox>
#include"send.h"
namespace Ui {
class addplan;
}
class addplan : public QDialog
{
    Q_OBJECT
private:
    int flag,type;
    QString m_id, name;
public:
    explicit addplan(int type,QString id,QString name="",QWidget *parent = nullptr);
    ~addplan();
    void closeEvent(QCloseEvent *);
signals:
    void back();
    void ok();

private:
    Ui::addplan *ui;
};

#endif // ADDPLAN_H
