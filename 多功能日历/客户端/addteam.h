#ifndef ADDTEAM_H
#define ADDTEAM_H

#include <QWidget>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include"send.h"
#include<QTimer>
namespace Ui {
class addteam;
}

class addteam : public QWidget
{
    Q_OBJECT
private:
    int flag;
    void creat();
    QString m_id;
public:
    explicit addteam(QString ,QWidget *parent = nullptr);
    ~addteam();
       void closeEvent(QCloseEvent *);
signals:
    void back(QString name);


private:
    Ui::addteam *ui;
};

#endif // ADDTEAM_H
